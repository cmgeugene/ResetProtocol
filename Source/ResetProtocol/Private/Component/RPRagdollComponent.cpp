// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/RPRagdollComponent.h"
#include "Character/RPPlayerCharacter.h"
#include "Frameworks/RPPlayerController.h"
#include "InteractableObject/RPCorpse.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

URPRagdollComponent::URPRagdollComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	bInitRagdoll = false;
	bIsRagdollOn = false;
}

void URPRagdollComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPRagdollComponent, bInitRagdoll);
}

void URPRagdollComponent::RagdollOn(AActor* Interactor)
{
	if (!Interactor)
	{
		return;
	}

	if (!bIsRagdollOn)
	{
		Multicast_RagdollOn();
	}
}

void URPRagdollComponent::RagdollOff(AActor* Interactor)
{
	if (!Interactor)
	{
		return;
	}

	if (bIsRagdollOn)
	{
		Multicast_RagdollOff();
		OnRagdollOffComplete(Interactor);
	}
}

void URPRagdollComponent::Multicast_RagdollOn_Implementation()
{
	if (ARPCorpse* OwnerActor = Cast<ARPCorpse>(GetOwner()))
	{
		OwnerActor->RootBox->SetSimulatePhysics(false);
		OwnerActor->RootBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		OwnerActor->SkeletalMeshComp->SetSimulatePhysics(true);
		OwnerActor->SkeletalMeshComp->AttachToComponent(OwnerActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		OwnerActor->SkeletalMeshComp->RegisterComponent();

		bIsRagdollOn = true;
	}
}

void URPRagdollComponent::Multicast_RagdollOff_Implementation()
{
	if (ARPCorpse* OwnerActor = Cast<ARPCorpse>(GetOwner()))
	{
		OwnerActor->RootBox->SetSimulatePhysics(true);
		OwnerActor->RootBox->SetCollisionProfileName(TEXT("ResetObjectRoot"));
		OwnerActor->SkeletalMeshComp->SetSimulatePhysics(false);
		// Ragdoll을 키면 움직임의 제어권이 물리 엔진으로 넘어감
		// - Ragdoll을 껐을 때 넘어간 제어권을 EAnimationMode로 가져오는 것
		OwnerActor->SkeletalMeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);

		// 위치 보정(Root가 SceneComp인 경우)
		FVector MeshLocation = OwnerActor->SkeletalMeshComp->GetRelativeLocation();
		FVector NewActorLocation = OwnerActor->GetActorLocation() + FVector(MeshLocation.X, MeshLocation.Y, 0.0f);
		OwnerActor->SetActorLocation(NewActorLocation, false, nullptr, ETeleportType::None);
		OwnerActor->SkeletalMeshComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -35.0f), FRotator(0.0f, -90.0f, 0.0f));

		bIsRagdollOn = false;
	}
}

void URPRagdollComponent::OnRagdollOffComplete(AActor* Interactor)
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(Interactor);
	if (!PlayerCharacter)
	{
		return;
	}
	ARPPlayerController* PlayerController = Cast<ARPPlayerController>(PlayerCharacter->GetController());
	if (!PlayerController)
	{
		return;
	}

	if (ARPBaseInteractableObject* OwnerActor = Cast<ARPBaseInteractableObject>(GetOwner()))
	{
		PlayerController->Server_OnResetSuccessHandle(OwnerActor->ObjectType);
	}
}

void URPRagdollComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetOwner()->HasAuthority())
	{
		bInitRagdoll = true;
		bIsRagdollOn = true;

		InitRagdoll(true);
	}
}

void URPRagdollComponent::OnRep_bInitRagdoll()
{
	InitRagdoll(bInitRagdoll);

	bIsRagdollOn = bInitRagdoll;
}

void URPRagdollComponent::InitRagdoll(bool bOn)
{
	if (ARPCorpse* OwnerActor = Cast<ARPCorpse>(GetOwner()))
	{
		if (bOn)
		{
			OwnerActor->RootBox->SetSimulatePhysics(false);
			OwnerActor->RootBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
			OwnerActor->SkeletalMeshComp->SetSimulatePhysics(true);
			OwnerActor->SkeletalMeshComp->AttachToComponent(OwnerActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			OwnerActor->SkeletalMeshComp->RegisterComponent();
		}
		else
		{
			OwnerActor->RootBox->SetSimulatePhysics(true);
			OwnerActor->RootBox->SetCollisionProfileName(TEXT("ResetObjectRoot"));
			OwnerActor->SkeletalMeshComp->SetSimulatePhysics(false);
			// Ragdoll을 키면 움직임의 제어권이 물리 엔진으로 넘어감
			// - Ragdoll을 껐을 때 넘어간 제어권을 EAnimationMode로 가져오는 것
			OwnerActor->SkeletalMeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);

			if (OwnerActor->HasAuthority())
			{
				// 위치 보정(Root가 SceneComp인 경우)
				FVector MeshLocation = OwnerActor->SkeletalMeshComp->GetRelativeLocation();
				FVector NewActorLocation = OwnerActor->GetActorLocation() + FVector(MeshLocation.X, MeshLocation.Y, 0.0f);
				OwnerActor->SetActorLocation(NewActorLocation, false, nullptr, ETeleportType::TeleportPhysics);
			
				OwnerActor->ForceNetUpdate();
			}

			OwnerActor->SkeletalMeshComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -35.0f), FRotator(0.0f, -90.0f, 0.0f));
		}
	}
}




// --------- Ragdoll On -----------
// OwnerActor->SkeletalMeshComp->SetPhysicsBlendWeight(0.0f);	// 애니 100%, 물리 0%

// --------- Ragdoll Off -------------
// 	OwnerActor->SkeletalMeshComp->SetPhysicsBlendWeight(1.0f);	// 애니 0%, 물리 100%
//	OwnerActor->SkeletalMeshComp->WakeAllRigidBodies();			// 확실히 쓰러지도록
//	
//	// 위치 보정
//	FVector MeshLocation = OwnerActor->SkeletalMeshComp->GetRelativeLocation();
//	FVector NewActorLocation = OwnerActor->GetActorLocation() + FVector(MeshLocation.X, MeshLocation.Y, 0.0f);
//	OwnerActor->SetActorLocation(NewActorLocation, false, nullptr, ETeleportType::None);
//	OwnerActor->SkeletalMeshComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));