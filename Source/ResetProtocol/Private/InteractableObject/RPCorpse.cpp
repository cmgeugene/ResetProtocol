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
}

void ARPCorpse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SkeletalMeshComp->IsSimulatingPhysics())
	{
		FVector MeshLocation = SkeletalMeshComp->GetRelativeLocation();
		FVector NewActorLocation = GetActorLocation() + FVector(MeshLocation.X, MeshLocation.Y, 0.0f);
		SetActorLocation(NewActorLocation, false, nullptr, ETeleportType::None);
	}
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
	}
}

void ARPCorpse::KeyReleaseInteract_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Ragdoll Off"));

	if (HasAuthority())
	{
		RagdollComp->Server_RagdollOff_Implementation();
	}
}

void ARPCorpse::BeginPlay()
{
	Super::BeginPlay();
}

void ARPCorpse::OnObjectOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (SkeletalMeshComp->IsSimulatingPhysics())
	{
		IRPKeyHoldInterface::Execute_KeyReleaseInteract(this, OtherActor);
	}
	else
	{
		IRPKeyHoldInterface::Execute_KeyHoldInteract(this, OtherActor);
	}
}
