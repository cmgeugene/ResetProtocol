// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CleaningTool/RPBaseCleaningTool.h"
#include "RPCleaningToolData.generated.h"


USTRUCT(BlueprintType)
struct FCleaningToolData
{
	GENERATED_BODY()

	//bool operator==(const FCleaningToolData& Other) const
	//{
	//	return ItemName == Other.ItemName;
	//}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Price;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ARPBaseCleaningTool> Class;
};

FORCEINLINE bool operator==(const FCleaningToolData& A, const FCleaningToolData& B)
{
	return A.ItemName == B.ItemName;
}

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
	UFUNCTION(BlueprintCallable)
	TArray<FCleaningToolData> GetDatas() { return Items; }

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FCleaningToolData> Items;
};


