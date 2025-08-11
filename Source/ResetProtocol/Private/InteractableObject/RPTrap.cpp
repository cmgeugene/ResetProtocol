// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject/RPTrap.h"
#include "Component/RPRepairableComponent.h"

ARPTrap::ARPTrap()
{
	bReplicates = true;
	SetReplicateMovement(true);

	BrokenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BrokenMesh"));
	BrokenMesh->SetupAttachment(SceneRoot);

	RepairComp = CreateDefaultSubobject<URPRepairableComponent>(TEXT("RepairComp"));
	RepairComp->SetIsReplicated(true);
}

void ARPTrap::KeyHoldInteract_Implementation(AActor* Interactor)
{
	RepairComp->Server_StartRepair();
}

void ARPTrap::KeyReleaseInteract_Implementation(AActor* Interactor)
{

}

void ARPTrap::BeginPlay()
{
	Super::BeginPlay();

	if (StaticMeshComp && StaticMeshComp->GetStaticMesh())
	{
		StaticMeshComp->SetVisibility(false);
		ActiveMesh = BrokenMesh;
	}
}
