// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RPPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Component/PlayerInfoComponent.h"

ARPPlayerState::ARPPlayerState()
{
	InfoComponent = CreateDefaultSubobject<UPlayerInfoComponent>(TEXT("PlayerInfoComponent"));
}

void ARPPlayerState::BeginPlay()
{
	Super::BeginPlay();

	/*if (HasAuthority())
	{
		PlayerInfo.PC = GetPlayerController();
		PlayerInfo.PlayerName = FText::FromString(GetPlayerName());
		PlayerInfo.PS = this;
		PlayerInfo.ReadyStatus = false;

	}*/
}

void ARPPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}
