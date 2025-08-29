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

		Multicast_Repair();

		OnCompleteRepair(Interactor);
	}
}

void URPRepairableComponent::Break(AActor* Interactor)
{
	if (bIsRepaired)
	{
		bIsRepaired = false;

		// 부서진 메시로 변경
		Multicast_Break();
	}
}

void URPRepairableComponent::Multicast_Repair_Implementation()
{
	if (ARPTrap* OwnerActor = Cast<ARPTrap>(GetOwner()))
	{
		if (OwnerActor->ActiveMesh == OwnerActor->BrokenMesh)
		{
			OwnerActor->BrokenMesh->SetVisibility(false);
			OwnerActor->StaticMeshComp->SetVisibility(true);

			OwnerActor->ActiveMesh = OwnerActor->StaticMeshComp;
			OwnerActor->bIsBroken = false;

			OwnerActor->AfterRepairProcess();
		}
	}
}

void URPRepairableComponent::Multicast_Break_Implementation()
{
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

void URPRepairableComponent::BeginPlay()
{
	Super::BeginPlay();

}

void URPRepairableComponent::OnCompleteRepair(AActor* Interactor)
{
	if (ARPBaseInteractableObject* OwnerObject = Cast<ARPBaseInteractableObject>(GetOwner()))
	{
		if (ARPTrap* OwnerActor = Cast<ARPTrap>(OwnerObject))
		{
			
		}

		OwnerObject->OnResetComplete(Interactor);
	}
}