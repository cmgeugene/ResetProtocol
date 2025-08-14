// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RPGameState.h"
#include "Net/UnrealNetwork.h"

const TArray<APlayerState*>& ARPGameState::GetPlayerList() const
{
	return PlayerArray;
}

void ARPGameState::OnRep_MatchPhase()
{
	// 블루프린트에서 구현
	OnRepMatchPhaseProcess();
}

void ARPGameState::SetMatchPhaseTo(const EMatchPhase NewPhase)
{
	MatchPhase = NewPhase;
}

void ARPGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGameState, MatchPhase);
}

