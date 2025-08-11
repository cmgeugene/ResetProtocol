// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject/RPCorpse.h"
#include "Component/RPMovableComponent.h"
#include "Component/RPRagdollComponent.h"
#include "Net/UnrealNetwork.h"

ARPCorpse::ARPCorpse()
{
	bReplicates = true;
	SetReplicateMovement(true);

	MoveComp = CreateDefaultSubobject<URPMovableComponent>(TEXT("MoveComp"));
	MoveComp->SetIsReplicated(true);

	RagdollComp = CreateDefaultSubobject<URPRagdollComponent>(TEXT("RagdollComp"));
	RagdollComp->SetIsReplicated(true);

	IsRagdollOn = false;
}

void ARPCorpse::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPCorpse, IsRagdollOn);
}

void ARPCorpse::DragInteract_Implementation(AActor* Interactor)
{
}

void ARPCorpse::DropInteract_Implementation(AActor* Interactor)
{
}

void ARPCorpse::KeyHoldInteract_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Ragdoll On"));
	
	if (HasAuthority())
	{
		RagdollComp->Server_RagdollOn_Implementation();
		IsRagdollOn = true;
	}
}

void ARPCorpse::KeyReleaseInteract_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Ragdoll Off"));

	if (HasAuthority())
	{
		RagdollComp->Server_RagdollOff_Implementation();
		IsRagdollOn = false;
	}
}

void ARPCorpse::OnObjectOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (IsRagdollOn)
	{
		IRPKeyHoldInterface::Execute_KeyReleaseInteract(this, OtherActor);
	}
	else
	{
		IRPKeyHoldInterface::Execute_KeyHoldInteract(this, OtherActor);
	}
}
