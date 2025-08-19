// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject/RPTrap.h"
#include "Component/RPRepairableComponent.h"
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

	bIsBroken = true;

	ObjectType = EInteractObjectType::Trap;
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
		BrokenMesh->SetVisibility(true);
		ActiveMesh = BrokenMesh;
		bIsBroken = true;
	}
}

void ARPTrap::OnObjectOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bIsBroken)
	{
		IRPKeyHoldInterface::Execute_KeyHoldInteract(this, OtherActor);
	}
	else
	{
		IRPKeyHoldInterface::Execute_KeyReleaseInteract(this, OtherActor);
	}
}
