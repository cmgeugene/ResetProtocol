// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject/RPCorpse.h"
#include "Component/RPMovableComponent.h"
#include "Component/RPRagdollComponent.h"

ARPCorpse::ARPCorpse()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	MoveComp = CreateDefaultSubobject<URPMovableComponent>(TEXT("MoveComp"));
	MoveComp->SetIsReplicated(true);

	RagdollComp = CreateDefaultSubobject<URPRagdollComponent>(TEXT("RagdollComp"));
	RagdollComp->SetIsReplicated(true);

	ObjectType = EInteractObjectType::Corpse;
	ObjectName = "Corpse";
}

void ARPCorpse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveComp->RootMode == ERPRootMode::Box && SkeletalMeshComp->IsSimulatingPhysics())
	{
		FVector NewActorLocation = SkeletalMeshComp->GetSocketLocation("Hips");
		SetActorLocation(NewActorLocation, false, nullptr, ETeleportType::None);
	}
}

void ARPCorpse::DragInteract_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Grab On"));

	if (HasAuthority())
	{
		if (!MoveComp->GetIsHeld() && !(RagdollComp->bIsRagdollOn))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Grab Start"));
			MoveComp->Grab(Interactor);
		}
	}
}

void ARPCorpse::DropInteract_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Grab Off"));

	if (HasAuthority())
	{
		if (MoveComp->GetIsHeld() && !(RagdollComp->bIsRagdollOn))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Drop Start"));
			MoveComp->Drop();
		}
	}
}

void ARPCorpse::KeyHoldInteract_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Ragdoll Off"));

	if (HasAuthority())
	{
		RagdollComp->RagdollOff(Interactor);
	}
}

void ARPCorpse::KeyReleaseInteract_Implementation(AActor* Interactor)
{
}

void ARPCorpse::BeginPlay()
{
	Super::BeginPlay();
}