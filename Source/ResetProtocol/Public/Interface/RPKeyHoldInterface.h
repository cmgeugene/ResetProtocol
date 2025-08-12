// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RPKeyHoldInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class URPKeyHoldInterface : public UInterface
{
	GENERATED_BODY()
};

class RESETPROTOCOL_API IRPKeyHoldInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void KeyHoldInteract(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void KeyReleaseInteract(AActor* Interactor);
};
