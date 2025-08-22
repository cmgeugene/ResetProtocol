// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject/RPTrash.h"
#include "Component/RPMovableComponent.h"
#include "Components/BoxComponent.h"

ARPTrash::ARPTrash()
{
	bReplicates = true;
	SetReplicateMovement(true);

	MoveComp = CreateDefaultSubobject<URPMovableComponent>(TEXT("MoveComp"));
	MoveComp->SetIsReplicated(true);

	ObjectType = EInteractObjectType::Trash;
	ObjectName = "Trash";
}

void ARPTrash::ClickInteract_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("PickUp"));

	if (HasAuthority())
	{
		MoveComp->Pickup(Interactor);
	}
}