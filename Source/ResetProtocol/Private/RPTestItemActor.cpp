// Fill out your copyright notice in the Description page of Project Settings.


#include "RPTestItemActor.h"

ARPTestItemActor::ARPTestItemActor()
{
    bReplicates = true;

    bAlwaysRelevant = true;
    SetReplicateMovement(true);
}

void ARPTestItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
