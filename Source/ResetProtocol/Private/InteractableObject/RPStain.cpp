// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject/RPStain.h"
#include "Component/RPCleanableComponent.h"
#include "Components/DecalComponent.h"
#include "Components/BoxComponent.h"

ARPStain::ARPStain()
{
	bReplicates = true;
	SetReplicateMovement(true);

	RootBox->SetSimulatePhysics(false);

	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Decal->SetupAttachment(RootComponent);

	CleanComp = CreateDefaultSubobject<URPCleanableComponent>(TEXT("CleanComp"));
	CleanComp->SetIsReplicated(true);

	ObjectType = EInteractObjectType::Stain;
	ObjectName = "Stain";
}

void ARPStain::ClickInteract_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, TEXT("Start Clean Stain"));

	if (HasAuthority())
	{
		CleanComp->Cleaning(Interactor);
	}
}

void ARPStain::BeginPlay()
{
	Super::BeginPlay();

	// ★Decal의 머티리얼을 DecalMID로 변경 : Scalar값들을 동적으로 바꾸기 위해
	if (UMaterialInterface* MaterialInterface = Decal->GetDecalMaterial())
	{
		DecalMID = UMaterialInstanceDynamic::Create(MaterialInterface, this);
		Decal->SetDecalMaterial(DecalMID);
	}

	// 델리게이트 구독
	if (CleanComp)
	{
		CleanComp->OnDirtChanged.AddDynamic(this, &ARPStain::HandleDirtChanged);
	}

	// Decal Opacity(=Dirty) 초기화
	if (DecalMID)
	{
		DecalMID->SetScalarParameterValue("DirtAmount", 1.0f);
	}
}

void ARPStain::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 델리게이트 해제(권장)
	if (CleanComp)
	{
		CleanComp->OnDirtChanged.RemoveDynamic(this, &ARPStain::HandleDirtChanged);
	}

	Super::EndPlay(EndPlayReason);
}

void ARPStain::HandleDirtChanged(float NewValue)
{
	if (DecalMID)
	{
		if (NewValue < 0.0f)
		{
			NewValue = 0.0f;
		}

		DecalMID->SetScalarParameterValue("DirtAmount", NewValue);
	}

	if (NewValue <= 0.0f)
	{
		if (HasAuthority())
		{
			TWeakObjectPtr<AActor> WeakActor = this;

			FTimerHandle DestoryHandle;
			GetWorld()->GetTimerManager().SetTimer(DestoryHandle,
				[WeakActor]()
				{
					if (WeakActor.IsValid())
					{
						WeakActor->Destroy();
					}
				},
				0.5f,
				false
			);
		}
	}
}