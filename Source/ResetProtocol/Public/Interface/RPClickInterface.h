// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RPClickInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class URPClickInterface : public UInterface
{
	GENERATED_BODY()
};

class RESETPROTOCOL_API IRPClickInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void ClickInteract(AActor* Interactor);
};
