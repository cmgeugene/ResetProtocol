// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/ItemDataBase.h"
#include "UI/Inventory/RPHotbarSlot.h"
#include "RPHotbarWidget.generated.h"

class UUniformGridPanel;

UCLASS()
class RESETPROTOCOL_API URPHotbarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URPHotbarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void UpdateUI();

	UFUNCTION()
	int GetLatestIndex();

	UFUNCTION()
	void OnHighlight(int CurrentIndex, int PreviousIndex);

	UFUNCTION()
	void OffHighlight(int CurrentIndex);

protected:

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* UGridPanel_Items;

	UPROPERTY(EditDefaultsOnly, Category = "Hotbar")
	TSubclassOf<URPHotbarSlot> HotbarSlotClass;

	UPROPERTY()
	int SlotIndex;

	UPROPERTY()
	int LatestIndex;

	UPROPERTY()
	bool IsFull;
};
