// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/RPCleanableComponent.h"
#include "Net/UnrealNetwork.h"

URPCleanableComponent::URPCleanableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	DirtAmount = 1.0f;
}

void URPCleanableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPCleanableComponent, DirtAmount);
}

void URPCleanableComponent::Cleaning()
{
	// 3 ~ 4번 상호작용을 해야 지울 수 있도록
	DirtAmount -= FMath::FRandRange(0.3, 0.4);

	// C++의 OnRep은 Server에서는 호출 안되기 때문에 직접 호출
	if (GetOwner()->HasAuthority())
	{
		OnDirtChanged.Broadcast(DirtAmount);
	}
}

void URPCleanableComponent::Completecleaning()
{
}

void URPCleanableComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void URPCleanableComponent::OnRep_DirtAmount()
{
	OnDirtChanged.Broadcast(DirtAmount);
}
