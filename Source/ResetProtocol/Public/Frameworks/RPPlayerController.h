// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Frameworks/RPPlayerState.h"
#include "RPPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class RESETPROTOCOL_API ARPPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "RPPlayerController")
	void RequestChangePlayerName(const FString& Name);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ChangePlayerName(const FString& Name);
	void Server_ChangePlayerName_Implementation(const FString& Name);
	bool Server_ChangePlayerName_Validate(const FString& Name);

	// 플레이어 스테이트가 할당되었을 때 호출 -> 컨트롤러에서 초기화 진행
	virtual void OnRep_PlayerState() override;
	
	UFUNCTION(BlueprintNativeEvent, Category = "RPPlayerController")
	void PlayerStateProcess();
	void PlayerStateProcess_Implementation();

	UFUNCTION(BlueprintCallable, Category = "RPPlayerController")
	void InitializeSequence();



private:

	UPROPERTY()
	bool bIsPlayerStateStandby = false;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
