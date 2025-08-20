// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject/RPScatteredObject.h"
#include "Component/RPMovableComponent.h"

ARPScatteredObject::ARPScatteredObject()
{
	bReplicates = true;
	SetReplicateMovement(true);

	MoveComp = CreateDefaultSubobject<URPMovableComponent>(TEXT("MoveComp"));
	MoveComp->SetIsReplicated(true);

	ObjectType = EInteractObjectType::ScatteredObject;
}

void ARPScatteredObject::DragInteract_Implementation(AActor* Interactor)
{
	if (!MoveComp->GetIsHeld())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Grab On"));

		MoveComp->Server_Grab_Implementation(Interactor);
	}
}

void ARPScatteredObject::DropInteract_Implementation(AActor* Interactor)
{
	if (MoveComp->GetIsHeld())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Grab Off"));

		MoveComp->Server_Drop_Implementation();
	}
}
