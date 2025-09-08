// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject/RPCorpse.h"
#include "Component/RPMovableComponent.h"
#include "Component/RPRagdollComponent.h"

ARPCorpse::ARPCorpse()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	MoveComp = CreateDefaultSubobject<URPMovableComponent>(TEXT("MoveComp"));
	MoveComp->SetIsReplicated(true);

	RagdollComp = CreateDefaultSubobject<URPRagdollComponent>(TEXT("RagdollComp"));
	RagdollComp->SetIsReplicated(true);

	ObjectType = EInteractObjectType::Corpse;
	ObjectName = "Corpse";
}

void ARPCorpse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveComp->RootMode == ERPRootMode::Box && SkeletalMeshComp->IsSimulatingPhysics())
	{
		if (HasAuthority())
		{
			// 서버에서만 위치 조정하기
			// - 클라에서도 하면, 
			//   - `SetReplicateMovement(true)` 때문에 서버로부터 받는 위치 1개
			//   - + 클라의 Tick에서 `SetActorLocation()` 때문에 생기는 위치 1개
			// - => 총 2개의 RootBox가 보임
			FVector NewActorLocation = SkeletalMeshComp->GetSocketLocation("Hips");
			SetActorLocation(NewActorLocation, false, nullptr, ETeleportType::None);
		}
		else
		{
			// 클라에서 뼈 전체의 위치를 리플리케이션 받는건 아니지만
			// 중심이 되는 Hips(pelvis)위치를 RootBox로 옮겨서 Mesh의 위치도 동기화
			// - 서버일 때 틱에서 RootBox를 서버 Mesh의 Hips의 Location으로 옮기면, 이 RootBox의 Location이 리플리케이션됨
			//   - 즉, RootBox의 Location == 서버 Mesh의 Hips Location
			// - 리플리케이션된 Location으로 클라 Mesh를 옮겨주면, 서버-클라의 Mesh가 동기화
			if (FBodyInstance* SkeletalBody = SkeletalMeshComp->GetBodyInstance(TEXT("Hips")))
			{
				FRotator HipTransform = SkeletalMeshComp->GetSocketRotation(TEXT("Hips"));
				FVector NewClientMeshLocation = GetActorLocation();
				FTransform ClientMeshTransform = FTransform(HipTransform, NewClientMeshLocation);

				SkeletalBody->SetBodyTransform(ClientMeshTransform, ETeleportType::TeleportPhysics);
				SkeletalBody->SetLinearVelocity(FVector::ZeroVector, false);
				SkeletalBody->SetAngularVelocityInRadians(FVector::ZeroVector, false);
			}
		}
	}
}

void ARPCorpse::DragInteract_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Grab On"));

	if (HasAuthority())
	{
		if (!MoveComp->GetIsHeld() && !(RagdollComp->bIsRagdollOn))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Grab Start"));
			MoveComp->Grab(Interactor);
		}
	}
}

void ARPCorpse::DropInteract_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Grab Off"));

	if (HasAuthority())
	{
		if (MoveComp->GetIsHeld() && !(RagdollComp->bIsRagdollOn))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Drop Start"));
			MoveComp->Drop();
		}
	}
}

void ARPCorpse::KeyHoldInteract_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Ragdoll Off"));

	if (HasAuthority())
	{
		RagdollComp->RagdollOff(Interactor);
	}
}

void ARPCorpse::KeyReleaseInteract_Implementation(AActor* Interactor)
{
}

void ARPCorpse::BeginPlay()
{
	Super::BeginPlay();
}