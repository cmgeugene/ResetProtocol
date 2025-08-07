// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RPPlayerState.generated.h"



USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Struct|PlayerInfo") 
	TObjectPtr<APlayerController> PC;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Struct|PlayerInfo") 
	FText PlayerName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Struct|PlayerInfo") 
	bool ReadyStatus;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Struct|PlayerInfo") 
	TObjectPtr<APlayerState> PS;

	bool operator==(const FPlayerInfo& Other) const
	{
		return PC == Other.PC && PlayerName.EqualTo(Other.PlayerName) && ReadyStatus == Other.ReadyStatus && PS == Other.PS;
	}

	bool operator!=(const FPlayerInfo& Other) const
	{
		return !(*this == Other);
	}

};
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerInfoChanged, const FPlayerInfo&, NewPlayerInfo);

UCLASS()
class RESETPROTOCOL_API ARPPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ARPPlayerState();
	
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerInfo, BlueprintReadOnly, Category = "PlayerInfo")
	FPlayerInfo PlayerInfo;

	UFUNCTION()
	void OnRep_PlayerInfo();

	// PlayerInfo 변경 시 UI에 알릴 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "PlayerInfo")
	FOnPlayerInfoChanged OnPlayerInfoChanged;

	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	void Server_SetPlayerInfo(const FPlayerInfo& NewInfo);

	UFUNCTION(BlueprintPure, Category = "PlayerInfo")
	FPlayerInfo GetPlayerInfo() const { return PlayerInfo; }

private:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
