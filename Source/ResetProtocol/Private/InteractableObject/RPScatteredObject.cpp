// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject/RPScatteredObject.h"
#include "Component/RPMovableComponent.h"

ARPScatteredObject::ARPScatteredObject()
{
	bReplicates = true;
	SetReplicateMovement(true);

	MoveComp = CreateDefaultSubobject<URPMovableComponent>(TEXT("MoveComp"));
	MoveComp->SetIsReplicated(true);
}

void ARPScatteredObject::DragInteract_Implementation(AActor* Interactor)
{

}

void ARPScatteredObject::DropInteract_Implementation(AActor* Interactor)
{

}