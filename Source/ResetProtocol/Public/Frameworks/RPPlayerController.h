// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Frameworks/RPPlayerState.h"
#include "InteractableObject/RPBaseInteractableObject.h"
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

	virtual void PostSeamlessTravel() override;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "RPPlayerController")
	void Server_NotifyClientReady();
	void Server_NotifyClientReady_Implementation();

	/*
	*  상호작용 당하는 아이템에서 호출할 핸들
	*  아이템->컨트롤러->게임스테이트 흐름으로 연결됩니다
	*/
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "RPPlayerController")
	void Server_OnResetSuccessHandle(const EInteractObjectType Type);
	void Server_OnResetSuccessHandle_Implementation(const EInteractObjectType Type);

	// Hologram
	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "RPPlayerController|Hologram")
	void Client_ActivateHologram(const AActor* GrabbedActor);
	void Client_ActivateHologram_Implementation(const AActor* GrabbedActor);
	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "RPPlayerController|Hologram")
	void Client_DeactivateHologram(const AActor* GrabbedActor);
	void Client_DeactivateHologram_Implementation(const AActor* GrabbedActor);

private:

	UPROPERTY()
	bool bIsPlayerStateStandby = false;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
