// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/RPMovableComponent.h"
#include "InteractableObject/RPBaseInteractableObject.h"
#include "InteractableObject/RPTrap.h"
#include "Character/RPPlayerCharacter.h"
#include "Frameworks/RPPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Net/UnrealNetwork.h"

URPMovableComponent::URPMovableComponent() : 
	GrabRange(125.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	Holder = nullptr;
	CacheGrabbedComp = nullptr;
	CacheRootBox = nullptr;
	CacheOwnerMesh = nullptr;

	bIsHeld = false;
	UserYawDeg = 0.f;
	UserPitchDeg = 0.f;

	RootMode = ERPRootMode::Box;
	bSwappingRoot = false;
	bIsPickup = false;
}

void URPMovableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPMovableComponent, bIsHeld);
	DOREPLIFETIME(URPMovableComponent, RootMode);
	DOREPLIFETIME(URPMovableComponent, UserYawDeg);
	DOREPLIFETIME(URPMovableComponent, UserPitchDeg);
}

void URPMovableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner() || !GetOwner()->HasAuthority() || !bIsHeld || !Holder.IsValid())
	{
		return;
	}

	if (UPhysicsHandleComponent* GrabComp = (Holder.Get())->FindComponentByClass<UPhysicsHandleComponent>())
	{
		if (GrabComp->GetGrabbedComponent() == CacheGrabbedComp.Get())
		{
			if (USceneComponent* GrabAnchor = FindAnchor(Holder.Get()))
			{
				FVector NewLocation = GrabAnchor->GetComponentLocation() + GrabAnchor->GetForwardVector() * GrabRange;
				//GrabComp->SetTargetLocation(NewLocation);

				// 누적 방법이 잘 안됨
				// - Anchor 대비 Target의 상대 회전을 사용
				//  - Grab 시 상대 회전을 구하고, Tick에서 갱신
				FQuat CurrAnchorRotation = GrabAnchor->GetComponentQuat();
				FQuat UserDelta = MakeUserDeltaQuatYawPitch(UserYawDeg, UserPitchDeg, GrabAnchor);
				FQuat Expected = (CurrAnchorRotation * UserDelta  * DeltaAnchorTarget).GetNormalized();

				GrabComp->SetTargetLocationAndRotation(NewLocation, Expected.Rotator());
			}
		}
	}
}

void URPMovableComponent::Pickup(AActor* Interactor)
{
	if (!Interactor)
	{
		return;
	}

	if (!bIsPickup)
	{
		bIsPickup = true;

		FTimerHandle DestoryTimerHandle;
		if (AActor* OwnerActor = GetOwner())
		{
			TWeakObjectPtr<AActor> WeakOwnerActor = OwnerActor;

			OnPickupComplete(Interactor);
			GetWorld()->GetTimerManager().SetTimer(DestoryTimerHandle,
				[WeakOwnerActor]()
				{
					if (WeakOwnerActor.IsValid())
					{
						WeakOwnerActor->Destroy();
					}
				},
				0.5f,
				false
			);
		}
	}
}

void URPMovableComponent::Grab(AActor* Interactor)
{
	// 캐릭터 앞에 붙이기
	if (!GetOwner())
	{
		return;
	}

	if (GetOwner()->HasAuthority()) 
	{
		Holder = Interactor;

		CacheRootBox = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
		CacheOwnerMesh = FindOwnerMeshComponent(GetOwner());
		if (!CacheRootBox.IsValid() || !CacheOwnerMesh.IsValid())
		{
			return;
		}

		UPrimitiveComponent* BeGrabbedComp = nullptr;
		if (IsRootPhysicsActive())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("IsRootPhysicsActive()"));
			BeGrabbedComp = CacheRootBox.Get();
		}
		else if(CacheOwnerMesh->IsSimulatingPhysics())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("IsSimulatingPhysics()"));
			BeGrabbedComp = CacheOwnerMesh.Get();
		}

		// Grab
		if (BeGrabbedComp == nullptr)
		{
			// Attach
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("BeGrabbedComp == nullptr"));
			UCameraComponent* CharacterCamera = Holder->FindComponentByClass<UCameraComponent>();
			if (CharacterCamera)
			{
				CacheRootBox->AttachToComponent(CharacterCamera, FAttachmentTransformRules::KeepWorldTransform);
				bIsHeld = true;
			}
		}
		else
		{	
			// Physics Handle
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("BeGrabbedComp != nullptr"));
			UPhysicsHandleComponent* GrabComp = Holder->FindComponentByClass<UPhysicsHandleComponent>();
			if (!GrabComp)
			{
				return;
			}

			if (CacheOwnerMesh.Get() == BeGrabbedComp)
			{
				RootMode = ERPRootMode::Mesh;
				ApplyRootSwap(RootMode, CacheOwnerMesh.Get(), CacheRootBox.Get());
			}

			// Grab
			FVector GrabLocation = BeGrabbedComp->GetComponentLocation();
			FRotator GrabRotation = BeGrabbedComp->GetComponentRotation();
			GrabComp->GrabComponentAtLocationWithRotation(BeGrabbedComp, NAME_None, GrabLocation, GrabRotation);

			OnGrabStart(Interactor, BeGrabbedComp);

			if (CacheOwnerMesh.Get() == BeGrabbedComp)
			{
				GetOwner()->ForceNetUpdate();
			}
		}
	}	
}

void URPMovableComponent::Drop()
{
	// 캐릭터에서 때기
	if (!GetOwner())
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		// Grab할 때 Root가 바뀐 상황을 고려해야함
		UPrimitiveComponent* GrabbedComp = nullptr;
		if (!Holder.IsValid())
		{
			return;
		}

		if (UPhysicsHandleComponent* GrabComp = (Holder.Get())->FindComponentByClass<UPhysicsHandleComponent>())
		{
			GrabbedComp = GrabComp->GetGrabbedComponent();
			if(GrabbedComp)
			{
				if (GrabbedComp == CacheGrabbedComp.Get())
				{
					GrabComp->ReleaseComponent();

					if (CacheOwnerMesh.Get() == GrabbedComp)
					{
						GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, TEXT("Grab Physics On Skeletal Mesh"));
						// 1) 핸들 릴리즈 이후
						const FTransform MeshTransform = CacheOwnerMesh->GetComponentTransform();

						// 2) 다음 프레임으로 미루기 (씬 업데이트 끝난 뒤 구조 변경)
						GetWorld()->GetTimerManager().SetTimerForNextTick(
							[this, MeshTransform]()
							{
								// 3) 먼저 액터를 메시 위치로 텔레포트
								GetOwner()->SetActorTransform(MeshTransform, false, nullptr, ETeleportType::TeleportPhysics);

								// 4) 루트 복귀
								RootMode = ERPRootMode::Box;
								ApplyRootSwap(RootMode, CacheRootBox.Get(), CacheOwnerMesh.Get());

								OnDropStart();

								GetOwner()->ForceNetUpdate();
							});
					}
					else
					{
						GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, TEXT("Grab RootBox, Mesh's Physics Off"));
						// 이 부분이 없으면 클라쪽에서 새롭게 물건을 들 때 한 번 클릭을 하고나서 Grab할 수 있음
						// - 이유 : 잡고 있던걸 Release 하면서 IsHeld값을 false로 바꾸지 않음
						//          그래서 다음 클릭에 Grab이 안되고 Drop만 되는데, Drop할때 Grab한 것이 없어서 아래에 Detach 쪽 로직을 실행
						//          Detatch 쪽 로직을 실행하면서 IsHeld가 False가 되어서 다음 클릭에 Grab을 할 수 있게됨
						// - 주 원인 : RootBox를 들었을 때의 상황을 고려하지 않았던 것(Scattered나 Ragdoll이 Off인 Corpse)
						OnDropStart();
					}
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, TEXT("Detach"));
				CacheRootBox->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

				OnDropStart();
			}
		}
	}
}

void URPMovableComponent::BeginPlay()
{
	Super::BeginPlay();

}

void URPMovableComponent::OnRep_RootMode()
{
	UPrimitiveComponent* TempRootBox = GetOwner()->FindComponentByClass<UBoxComponent>();
	UPrimitiveComponent* TempOwnerMesh = FindOwnerMeshComponent(GetOwner());
	if (!TempRootBox || !TempOwnerMesh)
	{
		return;
	}

	if (RootMode == ERPRootMode::Mesh)
	{
		ApplyRootSwap(RootMode, TempOwnerMesh, TempRootBox);
	}
	else
	{
		ApplyRootSwap(RootMode, TempRootBox, TempOwnerMesh);
	}
	
}

void URPMovableComponent::ApplyRootSwap(ERPRootMode Mode, UPrimitiveComponent* NewRoot, UPrimitiveComponent* Other)
{	
	if (!NewRoot || !Other)
	{
		return;
	}
	if (NewRoot == Other)
	{
		return;
	}
	if (bSwappingRoot)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	if (Owner->GetRootComponent() == NewRoot && Other->GetAttachParent() == NewRoot)
	{
		return;
	}

	bSwappingRoot = true;

	// Swap 대상 컴포넌트들 분리
	if (Other->IsAttachedTo(NewRoot))
	{
		Other->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
	if (NewRoot->IsAttachedTo(Other))
	{
		NewRoot->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
	// 분리 한 번 더
	if (Other->GetAttachParent())
	{
		Other->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
	if (NewRoot->GetAttachParent())
	{
		NewRoot->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
	const FTransform NewRootTransform = NewRoot->GetComponentTransform();

	// Root 교체
	Owner->SetRootComponent(NewRoot);

	// 컴포넌트 계층구조 유지
	if (Other->GetAttachParent() != NewRoot)
	{
		Other->AttachToComponent(NewRoot, FAttachmentTransformRules::KeepWorldTransform);
	}

	NewRoot->SetWorldTransform(NewRootTransform, false, nullptr, ETeleportType::TeleportPhysics);

	bSwappingRoot = false;
}

UMeshComponent* URPMovableComponent::FindOwnerMeshComponent(AActor* Owner) const
{
	if (!Owner)
	{
		return nullptr;
	}

	if (UStaticMeshComponent* StaticMesh = Owner->FindComponentByClass<UStaticMeshComponent>())
	{
		if (ARPTrap* TrapOwner = Cast<ARPTrap>(Owner))
		{
			StaticMesh = Cast<UStaticMeshComponent>(TrapOwner->ActiveMesh);
		}

		if (StaticMesh->GetStaticMesh())
		{
			return StaticMesh;
		}
	}
	
	if (USkeletalMeshComponent* SkeletalMesh = Owner->FindComponentByClass<USkeletalMeshComponent>())
	{
		if (SkeletalMesh->GetSkeletalMeshAsset())
		{
			return SkeletalMesh;
		}
	}
	
	return nullptr;
}

USceneComponent* URPMovableComponent::FindAnchor(AActor* Interactor) const
{
	if (!Interactor)
	{
		return nullptr;
	}

	if (UCameraComponent* CharacterCamera = Interactor->FindComponentByClass<UCameraComponent>())
	{
		return CharacterCamera;
	}

	return Interactor->GetRootComponent();
}

bool URPMovableComponent::IsRootPhysicsActive()
{
	if (UBoxComponent* OwnerRoot = Cast<UBoxComponent>(GetOwner()->GetRootComponent()))
	{
		bool bSim = OwnerRoot->IsSimulatingPhysics();

		FString Msg = FString::Printf(TEXT("Physics : %s"), bSim ? TEXT("True") : TEXT("False"));

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, Msg);

		return bSim;
	}

	return false;
}

//void URPMovableComponent::RotateObject(float X, float Y)
//{
//	if (!GetOwner() || !GetOwner()->HasAuthority() || !bIsHeld || !Holder.IsValid())
//	{
//		return;
//	}
//
//	if (UPhysicsHandleComponent* GrabComp = (Holder.Get())->FindComponentByClass<UPhysicsHandleComponent>())
//	{
//		if (GrabComp->GetGrabbedComponent() == CacheGrabbedComp.Get())
//		{
//			FVector CurrTargetLocation;
//			FRotator CurrTargetRotation;
//			GrabComp->GetTargetLocationAndRotation(CurrTargetLocation, CurrTargetRotation);
//
//			float newYaw = CurrTargetRotation.Yaw + X;
//			float newPitch = CurrTargetRotation.Pitch + Y;
//			FRotator NewTargetRotation = FRotator(newPitch, newYaw, CurrTargetRotation.Roll);
//
//			GrabComp->SetTargetRotation(NewTargetRotation);
//		}
//	}
//}

void URPMovableComponent::RotateYaw(float DeltaYawDeg)
{
	// 360도 회전
	UserYawDeg = NormalizeYaw(UserYawDeg + DeltaYawDeg);
}

void URPMovableComponent::RotatePitch(float DeltaPitchDeg)
{
	// Gimbal Lock 방지
	UserPitchDeg = FMath::Clamp(UserPitchDeg + DeltaPitchDeg, -80.f, 80.f);
}

float URPMovableComponent::NormalizeYaw(float InDeg) const
{
	float Y = FMath::Fmod(InDeg, 360.f);
	if (Y > 180.f)  Y -= 360.f;
	if (Y < -180.f) Y += 360.f;

	return Y;
}

FQuat URPMovableComponent::MakeUserDeltaQuatYawPitch(float YawDeg, float PitchDeg, USceneComponent* Anchor) const
{
	if (!Anchor || !Holder.IsValid())
	{
		return FQuat::Identity;
	}

	// 축 설정
	const FVector UpAxis = Anchor->GetUpVector();     // Yaw 기준 축
	const FVector RightAxis = Anchor->GetRightVector();  // Pitch 기준 축

	// 각각을 회전 쿼터니언로 변환
	const FQuat YawQuat = FQuat(UpAxis, FMath::DegreesToRadians(YawDeg));
	const FQuat PitchQuat = FQuat(RightAxis, FMath::DegreesToRadians(PitchDeg));

	// 조합 (Yaw → Pitch 순서)
	return (YawQuat * PitchQuat).GetNormalized();
}

void URPMovableComponent::OnGrabStart(AActor* Interactor, UPrimitiveComponent* BeGrabbedComp)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	// 상대 회전 저장
	FQuat AnchorRotation = FindAnchor(Interactor)->GetComponentQuat();
	FQuat BeGrabbedCompRotation = BeGrabbedComp->GetComponentQuat();
	DeltaAnchorTarget = (AnchorRotation.Inverse() * BeGrabbedCompRotation).GetNormalized();

	CacheGrabbedComp = BeGrabbedComp;
	bIsHeld = true;
	UserYawDeg = 0.0f;
	UserPitchDeg = 0.0f;
}

void URPMovableComponent::OnDropStart()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	bIsHeld = false;
	Holder = nullptr;
	CacheGrabbedComp.Reset();
	DeltaAnchorTarget = FQuat::Identity;
	UserYawDeg = 0.0f;
	UserPitchDeg = 0.0f;
}

void URPMovableComponent::OnPickupComplete(AActor* Interactor)
{
	if (ARPBaseInteractableObject* OwnerActor = Cast<ARPBaseInteractableObject>(GetOwner()))
	{
		OwnerActor->OnResetComplete(Interactor);
	}
}

void URPMovableComponent::OnPlaceComplete()
{
}















// ------------------- Drop -------------------------------
//UBoxComponent* RootBox = Cast<UBoxComponent>(GetOwner()->GetRootComponent());
//UPrimitiveComponent* OwnerMesh = FindOwnerMeshComponent(GetOwner());
//if (!RootBox || !OwnerMesh)
//{
//	return;
//}
//
//UPrimitiveComponent* GrabbedComp = nullptr;
//if (IsRootPhysicsActive())
//{
//	GrabbedComp = RootBox;
//}
//else if (OwnerMesh->IsSimulatingPhysics())
//{
//	GrabbedComp = OwnerMesh;
//}

//if (!GrabbedComp)
//{
//	GrabbedComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
//}
//else
//{
//	if (Holder.IsValid())
//	{
//		if (UPhysicsHandleComponent* GrabComp = (Holder.Get())->FindComponentByClass<UPhysicsHandleComponent>())
//		{
//			if (GrabComp->GetGrabbedComponent() == CacheGrabbedComp.Get())
//			{
//				GrabComp->ReleaseComponent();
//
//				if (OwnerMesh == GrabbedComp)
//				{
//					RootMode = ERPRootMode::Box;
//					ApplyRootSwap(RootMode);
//				}
//			}
//		}
//	}
//}




// -------------------- Grab한 Actor의 회전 -----------------------------
// 누적 : 이전 Rotation과 현재 Rotation의 차이를 구해서 Target에도 그 차이만큼 더해주면 됨
// - 정확하게는 회전한 각만큼 Target도 회전시키면 됨
//FQuat CurrAnchorRotation = GrabAnchor->GetComponentQuat();
//CurrAnchorRotation.Normalize();										// 정규화를 하면 누적 오차를 줄일 수 있음
//FQuat Delta = CurrAnchorRotation * PrevAnchorRotation.Inverse();	// 과거와 현재 회전의 차이 계산
//Delta.Normalize();

//FQuat CurrTargetRotation = GrabbedBox.Get()->GetComponentQuat();
//CurrTargetRotation.Normalize();
//FQuat NewTargetRotation = Delta * CurrTargetRotation;
//NewTargetRotation.Normalize();
//GrabComp->SetTargetLocationAndRotation(NewLocation, NewTargetRotation.Rotator());

//PrevAnchorRotation = CurrAnchorRotation;




// -------------- Constraint ----------------
//float PrevBoxMass;
//FVector PrevBoxInertiaScale;
//FVector PrevSkelInertiaScale;

// -------------- Constraint Grab ----------------
//GrabComp->SetTargetLocationAndRotation(GrabLocation, GrabRotation);
//
//// Handle 튜닝 : Anchor와 Target 사이
//GrabComp->bRotationConstrained = true;		// Physics Handle이 Target의 회전에 영향을 줄 수 있도록
//GrabComp->LinearStiffness = 8000.f;
//GrabComp->LinearDamping = 2500.f;
//GrabComp->AngularStiffness = 12000.f;
//GrabComp->AngularDamping = 6000.f;
//GrabComp->InterpolationSpeed = 60.f;
//
//// --- 2) 속도/각속도 초기화(잔류 에너지 제거) ---
//RootBox->SetPhysicsLinearVelocity(FVector::ZeroVector);
//RootBox->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
//OwnerMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
//OwnerMesh->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
//
//// Mesh의 Physics = true인 경우
//// - 제약(Constraint)로 Mesh와 Root를 하나로 묶음
//if (OwnerMesh->IsSimulatingPhysics())
//{
//	// Constraint 생성
//	if (!CarryConstraint)
//	{
//		CarryConstraint = NewObject<UPhysicsConstraintComponent>(GetOwner());
//		CarryConstraint->RegisterComponent();
//		CarryConstraint->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
//	}
//
//	OwnerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//
//	// 두 바디(Box ↔ Mesh) 묶기
//	CarryConstraint->SetConstrainedComponents(RootBox, NAME_None, OwnerMesh, FName("pelvis"));
//
//	// 두 바디 사이의 충돌 끄기
//	CarryConstraint->SetDisableCollision(true);
//
//	// Constraint 튜닝 : Target과 Target의 Mesh 사이
//	// - 제약 한계치 최소화 (둘이 겹치도록)
//	//   - 축별 상대 위치 이동 한계 제한
//	CarryConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Limited, 1.f);	// 허용 거리 +-1cm
//	CarryConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Limited, 1.f);
//	CarryConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Limited, 1.f);
//
//	// - 드라이브(Anchor)로 목표점(Target)을 붙들어 두기
//	CarryConstraint->SetLinearPositionDrive(true, true, true);
//	// Stiffness / Damping / ForceLimit 
//	// Stiffness(스프링 강성) : 클수록 목표 위치로 빠르고 강하게 당김
//	// Damping(감쇠) : 클수록 출렁임/진동을 빨리 죽임
//	// ForceLimit(최대 힘) : 0 = 무제한, n = 최대 n까지
//	CarryConstraint->SetLinearDriveParams(50000.f, 10000.f, 0.f);
//
//	// - 회전 드라이브 추가
//	CarryConstraint->SetAngularDriveMode(EAngularDriveMode::SLERP);		// SLERP : 두 바디의 회전을 통째로 일치
//	CarryConstraint->SetAngularOrientationDrive(true, true);			// SLERP는 Twist/Swing 모두 켜야함
//	CarryConstraint->SetAngularDriveParams(50000.f, 10000.f, 0.f);
//
//	// - Constraint Projection 활성화 : 지정한 오차값을 넘어갔을 때 설정한 Constraint 조건으로 강제로 맞춰줌
//	CarryConstraint->SetProjectionEnabled(true);
//	CarryConstraint->SetProjectionParams(1.0f, 1.0f, 30.0f, 60.0f);		// alpha(0~1) : 1에 가까울 수록 즉시 붙임, Tolerance : 오차 허용치
//
//  // - Box와 Skeletal Mesh의 질량 맞추기
//	if (FBodyInstance* PelvisBodyInstance = OwnerMesh->GetBodyInstance(TEXT("pelvis")))
//	{
//		const float PelvisMass = PelvisBodyInstance->GetBodyMass();
//
//		PrevBoxMass = RootBox->GetMass();
//		RootBox->SetMassOverrideInKg(NAME_None, PelvisMass, true);
//	}
//
//	// --- 5) 관성 스케일 보정(회전 떨림 완화) + 원복 대비 백업 ---
//	PrevBoxInertiaScale = RootBox->BodyInstance.InertiaTensorScale;
//	PrevSkelInertiaScale = OwnerMesh->BodyInstance.InertiaTensorScale;
//
//	RootBox->BodyInstance.InertiaTensorScale = FVector(0.7f); // 0.5~0.9 테스트
//	OwnerMesh->BodyInstance.InertiaTensorScale = FVector(0.5f); // 0.3~0.7 테스트
//}

// ----------------- Constraint Drop ---------------------
//if (CarryConstraint)
//{
//	CarryConstraint->BreakConstraint();		// 제약 해제
//	CarryConstraint->DestroyComponent();
//	CarryConstraint = nullptr;
//}
//
//RootBox->SetMassOverrideInKg(NAME_None, PrevBoxMass, false);
//PrevBoxMass = 0.0f;
//
//OwnerMesh->SetCollisionProfileName(TEXT("ResetObjectSkeletal"));
//RootBox->BodyInstance.InertiaTensorScale = PrevBoxInertiaScale;
//OwnerMesh->BodyInstance.InertiaTensorScale = PrevSkelInertiaScale;