// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/RPCleanableComponent.h"

URPCleanableComponent::URPCleanableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void URPCleanableComponent::StartCleaning()
{
}

void URPCleanableComponent::StopCleaning()
{
}

void URPCleanableComponent::Completecleaning()
{
}

void URPCleanableComponent::BeginPlay()
{
	Super::BeginPlay();
	
}
