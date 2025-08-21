// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "RPInteractWidgetSlot.generated.h"

class UTextBlock;
class UImage;
class UBorder;


UCLASS()
class RESETPROTOCOL_API URPInteractWidgetSlot : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;


private:
	UPROPERTY(meta = (BindWidget))
	UImage* IconImage;

	UPROPERTY(meta = (BindWidget))
	UBorder* BG_Input;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InputText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InteractText;

	
};
