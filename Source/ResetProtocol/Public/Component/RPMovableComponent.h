// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPMovableComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESETPROTOCOL_API URPMovableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URPMovableComponent();

	// RPC
	UFUNCTION(Server, Reliable)
	void Server_Pickup();
	UFUNCTION(Server, Reliable)
	void Server_StartDrag();
	UFUNCTION(Server, Reliable)
	void Server_StartDrop();

	void OnPlaceComplete();

protected:
	virtual void BeginPlay() override;

};
