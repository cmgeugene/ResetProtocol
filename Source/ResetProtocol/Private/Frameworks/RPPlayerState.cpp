// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RPPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Component/PlayerInfoComponent.h"

ARPPlayerState::ARPPlayerState()
{
	// BP컴포넌트를 추가하기 위해서 C++ 컴포넌트 기본 부착 해제
	//InfoComponent = CreateDefaultSubobject<UPlayerInfoComponent>(TEXT("PlayerInfoComponent"));
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



void ARPPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (ARPPlayerState* NewPlayerState = Cast<ARPPlayerState>(PlayerState))
	{
		NewPlayerState->StoredPlayerInfo = this->StoredPlayerInfo;
		NewPlayerState->Inventory = this->Inventory;

		UE_LOG(LogTemp, Display, TEXT("[RPPlayerState] CopyProperties : 속성 카피, current byte : %d"), NewPlayerState->StoredPlayerInfo.Bytes);
	}
}

void ARPPlayerState::StoreInventory(const TArray<FCleaningToolData>& NewInventory)
{
	Inventory = NewInventory;
	UE_LOG(LogTemp, Display, TEXT("[RPPlayerState] Stored Inventory : Length (%d)"), Inventory.Num());
}

void ARPPlayerState::StorePlayerInfo(const FPlayerInfo& NewPlayerInfo)
{
	StoredPlayerInfo = NewPlayerInfo;
	UE_LOG(LogTemp, Display, TEXT("[RPPlayerState] Stored Bytes in PlayerInfo : %d"), StoredPlayerInfo.Bytes);
}

void ARPPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPPlayerState, StoredPlayerInfo);
}
