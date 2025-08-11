// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPCleanableComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESETPROTOCOL_API URPCleanableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URPCleanableComponent();

	void StartCleaning();
	void StopCleaning();
	void Completecleaning();

protected:
	virtual void BeginPlay() override;

};
