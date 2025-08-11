// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/RPRepairableComponent.h"

URPRepairableComponent::URPRepairableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void URPRepairableComponent::Server_StartRepair_Implementation()
{

}

void URPRepairableComponent::Server_StopRepair_Implementation()
{
	// 얘가 필요한가?
}

void URPRepairableComponent::CompleteRepair()
{
}

void URPRepairableComponent::BeginPlay()
{
	Super::BeginPlay();

}