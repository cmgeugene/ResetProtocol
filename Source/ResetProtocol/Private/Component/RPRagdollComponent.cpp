// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/RPRagdollComponent.h"
#include "InteractableObject/RPCorpse.h"

URPRagdollComponent::URPRagdollComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void URPRagdollComponent::Server_RagdollOn_Implementation()
{
	Multicast_RagdollOn();
}

void URPRagdollComponent::Server_RagdollOff_Implementation()
{
	Multicast_RagdollOff();
}

void URPRagdollComponent::Multicast_RagdollOn_Implementation()
{
	if (ARPCorpse* OwnerActor = Cast<ARPCorpse>(GetOwner()))
	{
		OwnerActor->SkeletalMeshComp->SetSimulatePhysics(true);
		OwnerActor->SkeletalMeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
	}
}

void URPRagdollComponent::Multicast_RagdollOff_Implementation()
{
	if (ARPCorpse* OwnerActor = Cast<ARPCorpse>(GetOwner()))
	{
		OwnerActor->SkeletalMeshComp->SetSimulatePhysics(false);
		OwnerActor->SkeletalMeshComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));	// 기본값이 뭔지 보고 수정 필요
		// Ragdoll을 키면 움직임의 제어권이 물리 엔진으로 넘어감
		// - Ragdoll을 껐을 때 넘어간 제어권을 EAnimationMode로 가져오는 것
		OwnerActor->SkeletalMeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);

		// 위치 보정
		FVector MeshLocation = OwnerActor->SkeletalMeshComp->GetRelativeLocation();
		FVector NewActorLocation = OwnerActor->GetActorLocation() + FVector(MeshLocation.X, MeshLocation.Y, 0.0f);
		OwnerActor->SetActorLocation(NewActorLocation, false, nullptr, ETeleportType::None);
		OwnerActor->SkeletalMeshComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	}
}

void URPRagdollComponent::BeginPlay()
{
	Super::BeginPlay();
	
}