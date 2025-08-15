// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/RPRepairableComponent.h"
#include "InteractableObject/RPTrap.h"

URPRepairableComponent::URPRepairableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void URPRepairableComponent::Server_StartRepair_Implementation()
{
	if (ARPTrap* OwnerActor = Cast<ARPTrap>(GetOwner()))
	{
		if (OwnerActor->ActiveMesh == OwnerActor->BrokenMesh)
		{
			OwnerActor->BrokenMesh->SetVisibility(false);
			OwnerActor->StaticMeshComp->SetVisibility(true);

			OwnerActor->ActiveMesh = OwnerActor->StaticMeshComp;
			OwnerActor->bIsBroken = false;
		}
	}
}

void URPRepairableComponent::Server_StopRepair_Implementation()
{
	// 테스트를 위해 다시 부서진 메시로 변경
	if (ARPTrap* OwnerActor = Cast<ARPTrap>(GetOwner()))
	{
		if (OwnerActor->ActiveMesh == OwnerActor->StaticMeshComp)
		{
			OwnerActor->StaticMeshComp->SetVisibility(false);
			OwnerActor->BrokenMesh->SetVisibility(true);

			OwnerActor->ActiveMesh = OwnerActor->BrokenMesh;
			OwnerActor->bIsBroken = true;
		}
	}
}

void URPRepairableComponent::CompleteRepair()
{
}

void URPRepairableComponent::BeginPlay()
{
	Super::BeginPlay();

}