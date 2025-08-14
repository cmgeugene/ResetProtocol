// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Component/PlayerInfoComponent.h"
#include "RPPlayerState.generated.h"

/**
 * 
 */

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerInfoChanged, const FPlayerInfo&, NewPlayerInfo);

UCLASS()
class RESETPROTOCOL_API ARPPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ARPPlayerState();
	
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPlayerInfoComponent> InfoComponent;

	

public:
	UFUNCTION(BlueprintPure, Category = "PlayerState")
	FORCEINLINE UPlayerInfoComponent* GetPlayerInfoComponent() const { return InfoComponent; }

	virtual void CopyProperties(APlayerState* PlayerState) override;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "RPPlayerState")
	FPlayerInfo StoredPlayerInfo;

	UFUNCTION(BlueprintCallable)
	void StorePlayerInfo(const FPlayerInfo& NewPlayerInfo);

private:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
