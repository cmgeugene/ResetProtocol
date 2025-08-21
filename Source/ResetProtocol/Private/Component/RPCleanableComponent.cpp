// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/RPCleanableComponent.h"
#include "Character/RPPlayerCharacter.h"
#include "Frameworks/RPPlayerController.h"
#include "Net/UnrealNetwork.h"

URPCleanableComponent::URPCleanableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	DirtAmount = 1.0f;

	bIsClean = false;
}

void URPCleanableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPCleanableComponent, DirtAmount);
}

void URPCleanableComponent::Cleaning(AActor* Interactor)
{
	if (!Interactor)
	{
		return;
	}

	if (!bIsClean)
	{
		// 3 ~ 4번 상호작용을 해야 지울 수 있도록
		DirtAmount -= FMath::FRandRange(0.3, 0.4);

		if (DirtAmount <= 0.0f)
		{
			bIsClean = true;
			OnCleaningComplete(Interactor);
		}

		// C++의 OnRep은 Server에서는 호출 안되기 때문에 직접 호출
		if (GetOwner()->HasAuthority())
		{
			OnDirtChanged.Broadcast(DirtAmount);
		}
	}
}

void URPCleanableComponent::OnCleaningComplete(AActor* Interactor)
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(Interactor);
	if (!PlayerCharacter)
	{
		return;
	}
	ARPPlayerController* PlayerController = Cast<ARPPlayerController>(PlayerCharacter->GetController());
	if (!PlayerController)
	{
		return;
	}

	if (ARPBaseInteractableObject* OwnerActor = Cast<ARPBaseInteractableObject>(GetOwner()))
	{
		PlayerController->Server_OnResetSuccessHandle(OwnerActor->ObjectType);
	}
}

void URPCleanableComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void URPCleanableComponent::OnRep_DirtAmount()
{
	OnDirtChanged.Broadcast(DirtAmount);
}
