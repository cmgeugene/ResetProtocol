// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RPPlayerState.h"
#include "Net/UnrealNetwork.h"

ARPPlayerState::ARPPlayerState()
{
	bReplicates = true;
}

void ARPPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		PlayerInfo.PC = GetPlayerController();
		PlayerInfo.PlayerName = FText::FromString(GetPlayerName());
		PlayerInfo.PS = this;
		PlayerInfo.ReadyStatus = false;
	}
}

void ARPPlayerState::OnRep_PlayerInfo()
{
	OnPlayerInfoChanged.Broadcast(PlayerInfo);
	UE_LOG(LogTemp, Display, TEXT("[RPPlayerState] OnRep_PlayerInfo : Delegate broadcasted."));
}

void ARPPlayerState::Server_SetPlayerInfo(const FPlayerInfo& NewInfo)
{
	if (HasAuthority())
	{
		if (PlayerInfo != NewInfo)
		{
			PlayerInfo = NewInfo;

			OnRep_PlayerInfo();
		}
	}
}

void ARPPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPPlayerState, PlayerInfo);
}
