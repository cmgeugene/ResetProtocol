// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/RPInteractUIData.h"
#include "RPInteractWidget.generated.h"

class UTextBlock;
class UListView;
class UBorder;


UCLASS()
class RESETPROTOCOL_API URPInteractWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION()
	void SetText(const FString& string);

	UFUNCTION()
	void RefreshWidget();

	UFUNCTION()
	void AddList(const TArray<FInteractUIData>& Datas);

	UFUNCTION()
	void VisiblePrice(int Price);

	UFUNCTION()
	void InvisiblePrice();

protected:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InteractText;

	UPROPERTY(meta = (BindWidget))
	UListView* InteractList;

	UPROPERTY(meta = (BindWidget))
	UBorder* PriceBorder;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BytesText;

};
