// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject/RPTrap.h"
#include "Component/RPRepairableComponent.h"
#include "Net/UnrealNetwork.h"

ARPTrap::ARPTrap()
{
	bReplicates = true;
	SetReplicateMovement(true);

	BrokenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BrokenMesh"));
	BrokenMesh->SetupAttachment(SceneRoot);
	BrokenMesh->SetIsReplicated(true);

	RepairComp = CreateDefaultSubobject<URPRepairableComponent>(TEXT("RepairComp"));
	RepairComp->SetIsReplicated(true);

	IsBroken = true;
}

void ARPTrap::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPTrap, IsBroken);
}

void ARPTrap::KeyHoldInteract_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Repair"));

	if (HasAuthority())
	{
		RepairComp->Server_StartRepair_Implementation();
	}
}

void ARPTrap::KeyReleaseInteract_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Broken"));

	if (HasAuthority())
	{
		RepairComp->Server_StopRepair_Implementation();
	}
}

void ARPTrap::BeginPlay()
{
	Super::BeginPlay();

	if (StaticMeshComp && StaticMeshComp->GetStaticMesh())
	{
		StaticMeshComp->SetVisibility(false);
		ActiveMesh = BrokenMesh;
		IsBroken = true;
	}
}

void ARPTrap::OnObjectOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (IsBroken)
	{
		IRPKeyHoldInterface::Execute_KeyHoldInteract(this, OtherActor);
	}
	else
	{
		IRPKeyHoldInterface::Execute_KeyReleaseInteract(this, OtherActor);
	}
}
