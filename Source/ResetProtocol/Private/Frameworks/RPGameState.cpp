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
	OnMatchPhaseUpdated(MatchPhase);
}

void ARPGameState::Server_SetMatchPhaseTo_Implementation(const EMatchPhase NewPhase)
{
	MatchPhase = NewPhase;

	if (HasAuthority())
	{
		OnMatchPhaseUpdated(MatchPhase);
	}
}

void ARPGameState::OnMatchPhaseUpdated_Implementation(const EMatchPhase NewPhase)
{

}

void ARPGameState::OnResetSuccess_Implementation(const EInteractObjectType Type)
{
	FResetResult* FoundResult = ResetProcessResult.FindByKey(Type);
	
	if (FoundResult)
	{
		FoundResult->Count++;
	}
	else
	{
		FResetResult NewResult;
		NewResult.ObejctType = Type;
		NewResult.Count = 1;

		ResetProcessResult.Add(NewResult);
	}
}

void ARPGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGameState, MatchPhase);
}

