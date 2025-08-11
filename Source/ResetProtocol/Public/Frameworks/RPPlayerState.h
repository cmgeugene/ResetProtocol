// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RPPlayerState.generated.h"

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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UPlayerInfoComponent> InfoComponent;

public:
	UFUNCTION(BlueprintPure, Category = "PlayerState")
	FORCEINLINE class UPlayerInfoComponent* GetPlayerInfoComponent() const { return InfoComponent; }

private:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
