// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CleaningTool/RPBaseCleaningTool.h"
#include "RPCleaningToolData.generated.h"


USTRUCT()
struct FCleaningToolData
{
	GENERATED_BODY()

	bool operator==(const FCleaningToolData& Other) const
	{
		return ItemName == Other.ItemName;
	}

	UPROPERTY(EditDefaultsOnly)
	FName ItemName;

	UPROPERTY(EditDefaultsOnly)
	int Price;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ARPBaseCleaningTool> Class;
};

UCLASS()
class UCleaingToolDataUIObject : public UObject
{
	GENERATED_BODY()

public:
	FCleaningToolData ItemData;

};

UCLASS()
class RESETPROTOCOL_API URPCleaningToolData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FCleaningToolData> Items;
};


