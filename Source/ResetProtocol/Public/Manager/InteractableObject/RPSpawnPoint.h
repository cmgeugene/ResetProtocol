// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "GameplayTagContainer.h"
#include "RPSpawnPoint.generated.h"

UCLASS()
class RESETPROTOCOL_API ARPSpawnPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	ARPSpawnPoint();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	TArray<FGameplayTag> SpawnGroupTags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	int MaxSpawnCount;		// 최대 스폰 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	float Radius;			// 스폰 시 충돌 체크
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn|Id")
	FName SpawnPointId;		// CacheClass 채울 때 좀 더 효율적으로 하기 위한 식별자

	UPROPERTY(Transient)
	TArray<TSubclassOf<AActor>> CachedSpawnCandidateClasses;	
};
