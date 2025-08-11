// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInfoComponent.generated.h"

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerInfoComponentUpdated, const FPlayerInfo&, PlayerInfo);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESETPROTOCOL_API UPlayerInfoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerInfoComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	UPROPERTY(BlueprintAssignable, Category = "PlayerInfoComponent")
	FOnPlayerInfoComponentUpdated OnPlayerInfoComponentUpdated;

public:	

	UFUNCTION()
	void OnRep_Info();

	UPROPERTY(ReplicatedUsing = OnRep_Info, BlueprintReadWrite, Category = "PlayerInfoComponent")
	FPlayerInfo Info;

	// 클라이언트가 서버로 정보를 전송하기 위해 호출할 RPC
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SetPlayerInfo(const FPlayerInfo& NewInfo);
	void Server_SetPlayerInfo_Implementation(const FPlayerInfo& NewInfo);

	// 정보에 접근하기 위한 Getter
	UFUNCTION(BlueprintPure, Category = "PlayerInfo")
	const FPlayerInfo& GetPlayerInfo() const { return Info; }
	
};
