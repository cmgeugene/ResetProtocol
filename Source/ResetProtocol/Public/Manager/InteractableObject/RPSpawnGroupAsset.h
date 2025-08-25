// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RPSpawnGroupAsset.generated.h"

UCLASS()
class RESETPROTOCOL_API URPSpawnGroupAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnGroup|Tag")
	FGameplayTag GroupTag;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "SpawnGroup|Objects")
	TArray<TSubclassOf<AActor>> SpawnCandidateClasses;

	/** AssetManager에서 고정 타입명으로 인덱싱 (프로젝트 세팅과 일치) */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("RPSpawnGroup"), GetFName());
	}
};
