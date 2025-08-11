// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject/RPStain.h"
#include "Component/RPCleanableComponent.h"

ARPStain::ARPStain()
{
	bReplicates = true;
	SetReplicateMovement(true);

	CleanComp = CreateDefaultSubobject<URPCleanableComponent>(TEXT("CleanComp"));
	CleanComp->SetIsReplicated(true);
}

void ARPStain::ClickInteract_Implementation(AActor* Interactor)
{

}
