// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject/RPTrap.h"
#include "Component/RPRepairableComponent.h"
#include "Component/GlitchNoiseComponent.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

ARPTrap::ARPTrap()
{
	bReplicates = true;
	SetReplicateMovement(true);

	BrokenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BrokenMesh"));
	BrokenMesh->SetupAttachment(RootComponent);
	BrokenMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BrokenMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	BrokenMesh->SetVisibility(false);
	BrokenMesh->SetIsReplicated(true);

	RepairComp = CreateDefaultSubobject<URPRepairableComponent>(TEXT("RepairComp"));
	RepairComp->SetIsReplicated(true);

	bIsBroken = false;

	ObjectType = EInteractObjectType::Trap;
	ObjectName = "Trap";
}

void ARPTrap::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPTrap, bIsBroken);
}

void ARPTrap::KeyHoldInteract_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Repair"));

	if (HasAuthority())
	{
		RepairComp->Repair(Interactor);
	}
}

void ARPTrap::KeyReleaseInteract_Implementation(AActor* Interactor)
{
}

void ARPTrap::AfterRepairProcess_Implementation()
{
}

void ARPTrap::OnRep_IsBroken()
{
	if (StaticMeshComp && StaticMeshComp->GetStaticMesh())
	{
		StaticMeshComp->SetVisibility(false);
		BrokenMesh->SetVisibility(true);
		ActiveMesh = BrokenMesh;

		if (GlitchNoiseComp)
		{
			GlitchNoiseComp->GlitchMeshUpdate();
		}

		AfterBeginPlayProcess();
	}
}

void ARPTrap::BeginPlay()
{
	Super::BeginPlay();

	RootBox->SetSimulatePhysics(false);

	if (HasAuthority())
	{
		int32 BrokenProbability = FMath::RandRange(0, 100);

		if (BrokenProbability < 80)
		{
			bIsBroken = true;
			OnRep_IsBroken();
		}
	}
}