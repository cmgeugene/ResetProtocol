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
	if (ARPCorpse* OwnerActor = Cast<ARPCorpse>(GetOwner()))
	{
		OwnerActor->SkeletalMeshComp->SetSimulatePhysics(true);
		OwnerActor->SkeletalMeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
	}
}

void URPRagdollComponent::Server_RagdollOff_Implementation()
{
	if (ARPCorpse* OwnerActor = Cast<ARPCorpse>(GetOwner()))
	{
		OwnerActor->SkeletalMeshComp->SetSimulatePhysics(false);
		OwnerActor->SkeletalMeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));	// 기본값이 뭔지 보고 수정 필요

		// Ragdoll을 키면 움직임의 제어권이 물리 엔진으로 넘어감
		// - Ragdoll을 껐을 때 넘어간 제어권을 EAnimationMode로 가져오는 것
		OwnerActor->SkeletalMeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		OwnerActor->SkeletalMeshComp->InitAnim(true);	// ABP 강제 재초기화

		FVector Location = OwnerActor->GetActorLocation();
		FRotator Rotation = FRotator::ZeroRotator;

		OwnerActor->SetActorLocationAndRotation(Location, Rotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void URPRagdollComponent::BeginPlay()
{
	Super::BeginPlay();
	
}