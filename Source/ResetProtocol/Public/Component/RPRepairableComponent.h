// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPRepairableComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESETPROTOCOL_API URPRepairableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URPRepairableComponent();

	UFUNCTION(Server, Reliable)
	void Server_StartRepair();
	UFUNCTION(Server, Reliable)
	void Server_StopRepair();

	void CompleteRepair();

protected:
	virtual void BeginPlay() override;
	
};
