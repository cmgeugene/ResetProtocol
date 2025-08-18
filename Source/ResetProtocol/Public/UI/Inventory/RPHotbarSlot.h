// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/ItemDataBase.h"
#include "Data/RPCleaningToolData.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "RPHotbarSlot.generated.h"



UCLASS()
class RESETPROTOCOL_API URPHotbarSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	URPHotbarSlot(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void SetData(const FCleaningToolData& _ItemData, int _SlotIndexs);

	UFUNCTION()
	void HighlightOn();

	UFUNCTION()
	void HighlightOff();
protected:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_KeyIndex;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Slot;

	UPROPERTY(meta = (BindWidget))
	UOverlay* Overlay_Image;

	UPROPERTY(meta = (BindWidget))
	UImage* Highlight;

	UPROPERTY(meta = (BindWidget))
	UImage* IconImage;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FCleaningToolData ItemData;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	int SlotIndexs;
};
