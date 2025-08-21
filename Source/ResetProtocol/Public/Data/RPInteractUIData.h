// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RPInteractUIData.generated.h"

USTRUCT()
struct FInteractUIData
{
	GENERATED_BODY()

	bool operator==(const FInteractUIData& Other) const
	{
		return ItemName == Other.ItemName;
	}

	bool operator==(const FName& OtherName) const
	{
		return ItemName == OtherName;
	}

	UPROPERTY(EditDefaultsOnly)
	FName ItemName;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly)
	FString InputText;

	UPROPERTY(EditDefaultsOnly)
	FString InteractText;
};

UCLASS()
class UInteractDataUIObject : public UObject
{
	GENERATED_BODY()

public:
	FInteractUIData ItemData;
};

UCLASS()
class RESETPROTOCOL_API URPInteractUIData : public UDataAsset
{
	GENERATED_BODY()

public:
	FInteractUIData FindItem(const FName& ItemName);

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FInteractUIData> Items;
};