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
}

void ARPTrash::ClickInteract_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Ragdoll Off"));

	if (HasAuthority())
	{
		MoveComp->Server_Pickup_Implementation();
	}
}

void ARPTrash::OnObjectOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	IRPClickInterface::Execute_ClickInteract(this, OtherActor);
}
