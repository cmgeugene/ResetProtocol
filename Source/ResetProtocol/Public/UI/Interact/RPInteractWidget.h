// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPInteractWidget.generated.h"

class UTextBlock;

UCLASS()
class RESETPROTOCOL_API URPInteractWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION()
	void SetText(const FString& string);


protected:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InteractText;
	
};
