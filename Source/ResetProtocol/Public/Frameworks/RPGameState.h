// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RPGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerListUpdatedSignature);

/**
 * 
 */
UCLASS()
class RESETPROTOCOL_API ARPGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	// OnPostLogin, OnLogout 이후 호출되는 델리게이트. 플레이어 수 변동이 있을 때 호출
	UPROPERTY(BlueprintAssignable, Category = "RPGameState")
	FOnPlayerListUpdatedSignature OnPlayerListUpdated;

	// 기본 제공 PlayerArray에 대한 Getter
	UFUNCTION(BlueprintCallable, Category = "RPGameState")
	const TArray<APlayerState*>& GetPlayerList() const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnPlayerListUpdated();
	void Multicast_OnPlayerListUpdated_Implementation();

	
};
