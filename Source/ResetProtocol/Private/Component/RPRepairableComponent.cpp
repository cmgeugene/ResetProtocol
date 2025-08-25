// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/RPRepairableComponent.h"
#include "InteractableObject/RPTrap.h"
#include "Character/RPPlayerCharacter.h"
#include "Frameworks/RPPlayerController.h"

URPRepairableComponent::URPRepairableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	bIsRepaired = false;
}

void URPRepairableComponent::Repair(AActor* Interactor)
{
	if (!Interactor)
	{
		return;
	}

	if (!bIsRepaired) 
	{
		bIsRepaired = true;

		if (ARPTrap* OwnerActor = Cast<ARPTrap>(GetOwner()))
		{
			if (OwnerActor->ActiveMesh == OwnerActor->BrokenMesh)
			{
				OwnerActor->BrokenMesh->SetVisibility(false);
				OwnerActor->StaticMeshComp->SetVisibility(true);

				OwnerActor->ActiveMesh = OwnerActor->StaticMeshComp;
				OwnerActor->bIsBroken = false;

				OnCompleteRepair(Interactor);
			}
		}
	}
}

void URPRepairableComponent::Break(AActor* Interactor)
{
	if (bIsRepaired)
	{
		bIsRepaired = false;

		// 부서진 메시로 변경
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
}

void URPRepairableComponent::OnCompleteRepair(AActor* Interactor)
{
	if (ARPBaseInteractableObject* OwnerActor = Cast<ARPBaseInteractableObject>(GetOwner()))
	{
		OwnerActor->OnResetComplete(Interactor);
	}
}

void URPRepairableComponent::BeginPlay()
{
	Super::BeginPlay();

}