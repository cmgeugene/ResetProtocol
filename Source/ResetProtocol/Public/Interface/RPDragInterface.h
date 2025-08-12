// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RPDragInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class URPDragInterface : public UInterface
{
	GENERATED_BODY()
};

class RESETPROTOCOL_API IRPDragInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void DragInteract(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void DropInteract(AActor* Interactor);
};
