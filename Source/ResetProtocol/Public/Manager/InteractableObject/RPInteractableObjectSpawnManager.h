// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RPInteractableObjectSpawnManager.generated.h"

class ARPSpawnPoint;
class URPSpawnGroupAsset;

UCLASS()
class RESETPROTOCOL_API ARPInteractableObjectSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ARPInteractableObjectSpawnManager();

	// 수동
	UFUNCTION(BlueprintCallable)
	void SpawnAllOnce();
	UFUNCTION(BlueprintCallable)
	void ResetAll();

protected:
	virtual void BeginPlay() override;

private:
	// 스폰 추적 델리게이트
	UFUNCTION()
	void HandleSpawnedActorDestroyed(AActor* DestroyedActor);

	// AssetManger에서 전체 그룹 에셋 로드 - 캐시용
	void LoadAllGroupsOnce();

	// 전체 스폰
	void RunInitialSpawn();
	// 각 포인트에서 랜덤으로 스폰
	bool SpawnFromPoint(ARPSpawnPoint* Point, FRandomStream& Stream);
	bool FindSpawnTransform(ARPSpawnPoint* Point, FTransform& Out) const;

public:
	UPROPERTY(EditAnywhere, Category = "Spawn|Timing")
	bool bIsSpawnAtBeginPlay;
	UPROPERTY(EditAnywhere, Category = "Spawn|Timing")
	float InitDelay;
	UPROPERTY(EditAnyWhere, Category = "Spawn|Random")
	int32 RandomSeed;
	// 태그 비교 시 부모 매칭 허용
	UPROPERTY(EditAnywhere, Category = "Spawn|Tags")
	bool bMatchParentTags;
	// Overlap 검사 채널
	//UPROPERTY(EditAnywhere, Category = "Spawn|Collision")
	//TEnumAsByte<ECollisionChannel> OverlapChannel;	

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<URPSpawnGroupAsset>> AllGroups;
	UPROPERTY(Transient)
	TMap<FName, TWeakObjectPtr<ARPSpawnPoint>> AllSpawnPointIds;

	// 스폰 추적
	TMap<TWeakObjectPtr<AActor>, TWeakObjectPtr<ARPSpawnPoint>> SpawnedToPoint;
	TMap<TWeakObjectPtr<ARPSpawnPoint>, int> PointCounts;
	bool bIsAlreadySpawned;
};