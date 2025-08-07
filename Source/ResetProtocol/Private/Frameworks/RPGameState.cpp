// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RPGameState.h"
#include "Net/UnrealNetwork.h"



void ARPGameState::Multicast_OnPlayerListUpdated_Implementation()
{
	OnPlayerListUpdated.Broadcast();
}

const TArray<APlayerState*>& ARPGameState::GetPlayerList() const
{
	return PlayerArray;
}


