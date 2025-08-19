// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/RPMovableComponent.h"
#include "InteractableObject/RPBaseInteractableObject.h"
#include "InteractableObject/RPTrap.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Net/UnrealNetwork.h"

URPMovableComponent::URPMovableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	Holder = nullptr;
	GrabbedBox = nullptr;
	bIsHeld = false;
}

void URPMovableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPMovableComponent, bIsHeld);
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
		if (GrabComp->GetGrabbedComponent() == GrabbedBox.Get())
		{
			if (USceneComponent* GrabAnchor = FindAnchor(Holder.Get()))
			{
				FVector NewLocation = GrabAnchor->GetComponentLocation() + GrabAnchor->GetForwardVector() * 250.0f;
				
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

				// 누적 방법이 잘 안됨
				// - Anchor 대비 Target의 상대 회전을 사용
				//  - Grab 시 상대 회전을 구하고, Tick에서 갱신
				FQuat CurrAnchorRotation = GrabAnchor->GetComponentQuat();
				FQuat Expected = (CurrAnchorRotation * DeltaAnchorTarget).GetNormalized();

				GrabComp->SetTargetLocationAndRotation(NewLocation, Expected.Rotator());
			}
		}
	}
}

void URPMovableComponent::Server_Pickup_Implementation()
{
	FTimerHandle DestoryTimerHandle;
	if (AActor* OwnerActor = GetOwner())
	{
		TWeakObjectPtr<AActor> WeakOwnerActor = OwnerActor;

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

void URPMovableComponent::Server_Grab_Implementation(AActor* Interactor)
{
	// 캐릭터 앞에 붙이기
	if (!GetOwner())
	{
		return;
	}

	if (GetOwner()->HasAuthority()) 
	{
		Holder = Interactor;

		UBoxComponent* RootBox = Cast<UBoxComponent>(GetOwner()->GetRootComponent());
		if (!RootBox)
		{
			return;
		}

		UMeshComponent* OwnerMesh = FindOwnerMeshComponent(GetOwner());
		if (!OwnerMesh)
		{
			return;
		}

		if (!IsRootPhysicsActive())
		{
			UCameraComponent* CharacterCamera = Holder->FindComponentByClass<UCameraComponent>();
			if (CharacterCamera)
			{
				RootBox->AttachToComponent(CharacterCamera, FAttachmentTransformRules::KeepWorldTransform);
				bIsHeld = true;
			}
		}
		else
		{
			FVector GrabLocation = RootBox->GetComponentLocation();
			FRotator GrabRotation = RootBox->GetComponentRotation();
			
			if (UPhysicsHandleComponent* GrabComp = Holder->FindComponentByClass<UPhysicsHandleComponent>())
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("SkeletalMesh : Physics Off"));

				GrabComp->GrabComponentAtLocationWithRotation(RootBox, NAME_None, GrabLocation, GrabRotation);

				// Mesh의 Physics = true인 경우
				// - 제약(Constraint)로 Mesh와 Root를 하나로 묶음
				if (OwnerMesh->IsSimulatingPhysics())
				{

				}

				FQuat AnchorRotation = FindAnchor(Interactor)->GetComponentQuat();
				FQuat BoxRotation = RootBox->GetComponentQuat();
				DeltaAnchorTarget = (AnchorRotation.Inverse() * BoxRotation).GetNormalized();

				GrabbedBox = RootBox;
				bIsHeld = true;
			}
		}
	}	
}

void URPMovableComponent::Server_Drop_Implementation()
{
	// 캐릭터에서 때기
	if (!GetOwner())
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		UBoxComponent* RootBox = Cast<UBoxComponent>(GetOwner()->GetRootComponent());
		if (!RootBox)
		{
			return;
		}

		UMeshComponent* OwnerMesh = FindOwnerMeshComponent(GetOwner());
		if (!OwnerMesh)
		{
			return;
		}

		if (!IsRootPhysicsActive())
		{
			RootBox->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}
		else
		{
			if (Holder.IsValid())
			{
				if (UPhysicsHandleComponent* GrabComp = (Holder.Get())->FindComponentByClass<UPhysicsHandleComponent>())
				{
					if (Cast<UBoxComponent>(GrabComp->GetGrabbedComponent()) == GrabbedBox.Get())
					{
						GrabComp->ReleaseComponent();
					}
				}
			}
		}

		bIsHeld = false;
		Holder = nullptr;
		GrabbedBox = nullptr;
	}
}

void URPMovableComponent::BeginPlay()
{
	Super::BeginPlay();

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

void URPMovableComponent::OnPlaceComplete()
{
}