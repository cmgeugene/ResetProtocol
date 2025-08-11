// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPRagdollComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESETPROTOCOL_API URPRagdollComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URPRagdollComponent();

	// RPC
	UFUNCTION(Server, Reliable)
	void Server_RagdollOn();
	UFUNCTION(Server, Reliable)
	void Server_RagdollOff();

protected:
	virtual void BeginPlay() override;
	
};
