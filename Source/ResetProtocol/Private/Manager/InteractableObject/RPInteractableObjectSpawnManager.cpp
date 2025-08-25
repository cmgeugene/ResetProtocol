// Fill out your copyright notice in the Description page of Project Settings.

#include "Manager/InteractableObject/RPInteractableObjectSpawnManager.h"
#include "Manager/InteractableObject/RPSpawnGroupAsset.h"
#include "Manager/InteractableObject/RPSpawnPoint.h"
#include "InteractableObject/RPBaseInteractableObject.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

ARPInteractableObjectSpawnManager::ARPInteractableObjectSpawnManager() :
	bIsSpawnAtBeginPlay(true),
	InitDelay(0.1f),
	RandomSeed(4885),
	bMatchParentTags(false),
	//OverlapChannel(ECC_Pawn),
	bIsAlreadySpawned(false)
{
	bReplicates = false;
	SetReplicateMovement(false);
	PrimaryActorTick.bCanEverTick = false;
}

void ARPInteractableObjectSpawnManager::SpawnAllOnce()
{
	if (!HasAuthority())
	{
		return;
	}

	LoadAllGroupsOnce();
	RunInitialSpawn();
}

void ARPInteractableObjectSpawnManager::ResetAll()
{
	if (!HasAuthority())
	{
		return;
	}

	for (TPair<TWeakObjectPtr<AActor>, TWeakObjectPtr<ARPSpawnPoint>> Pair : SpawnedToPoint)
	{
		if (Pair.Key.IsValid())
		{
			// 델리게이트 해제
			Pair.Key.Get()->OnDestroyed.RemoveDynamic(this, &ARPInteractableObjectSpawnManager::HandleSpawnedActorDestroyed);
			Pair.Key.Get()->Destroy();
		}
	}

	SpawnedToPoint.Empty();
	PointCounts.Empty();
	bIsAlreadySpawned = false;
}

void ARPInteractableObjectSpawnManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (!HasAuthority())
	{
		return;
	}

	RandomSeed = FMath::RandRange(0, 9999);

	LoadAllGroupsOnce();

	if (bIsSpawnAtBeginPlay)
	{
		// 기본적으로 레벨의 환경들이 배치되고 나서 오브젝트들을 스폰하기 위해
		FTimerHandle SpawnTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ARPInteractableObjectSpawnManager::RunInitialSpawn, InitDelay, false);
	}
}

void ARPInteractableObjectSpawnManager::LoadAllGroupsOnce()
{
	if (AllGroups.Num() > 0)
	{
		return;
	}

	UAssetManager& SpawnGroupAssetManager = UAssetManager::Get();

	//FPrimaryAssetTypeInfo Info;
	//const bool bHasType = SpawnGroupAssetManager.GetPrimaryAssetTypeInfo(FPrimaryAssetType(TEXT("RPSpawnGroup")), Info);
	//UE_LOG(LogTemp, Warning, TEXT("[Spawn] HasType=%d BaseClass=%s Dirs=%d HasBP=%d CookRule=%d"),
	//	bHasType,
	//	*GetNameSafe(Info.AssetBaseClassLoaded),
	//	Info.GetDirectories().Num(),
	//	Info.bHasBlueprintClasses,
	//	(int32)Info.Rules.CookRule);

	TArray<FPrimaryAssetId> SpawnGroupAssetIds;
	SpawnGroupAssetManager.GetPrimaryAssetIdList(FPrimaryAssetType(TEXT("RPSpawnGroup")), SpawnGroupAssetIds);

	for (const FPrimaryAssetId& Id : SpawnGroupAssetIds)
	{
		// PrimaryAsset이 메모리에 로드되었는지
		// - GetPrimaryAssetObject() : 에셋 매니저가 관리 중인 에셋들(매니저 캐시) 중에서 찾음
		UObject* Obj = SpawnGroupAssetManager.GetPrimaryAssetObject(Id);
		if (!Obj)
		{
			// 로드되어 있지 않으면, 소프트 경로로 한 번 더 로드
			// - ResolveObject() : 전역적으로 메모리에 올라온 UObject 전체를 확인
			const FSoftObjectPath Path = SpawnGroupAssetManager.GetPrimaryAssetPath(Id);
			Obj = Path.ResolveObject() ? Path.ResolveObject() : Path.TryLoad();
		}

		if (URPSpawnGroupAsset* GroupAsset = Cast<URPSpawnGroupAsset>(Obj))
		{
			AllGroups.Add(GroupAsset);
		}
	}
}

void ARPInteractableObjectSpawnManager::RunInitialSpawn()
{
	if (!HasAuthority() || bIsAlreadySpawned)
	{
		return;
	}

	FRandomStream Stream;
	Stream.Initialize(RandomSeed);

	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARPSpawnPoint::StaticClass(), SpawnPoints);

	for (AActor* Actor : SpawnPoints)
	{
		ARPSpawnPoint* SpawnPoint = Cast<ARPSpawnPoint>(Actor);
		if (!SpawnPoint)
		{
			continue;
		}
		if (SpawnPoint->SpawnGroupTags.IsEmpty())
		{
			continue;
		}

		// SpawnPoint의 Cache 채우기
		if (SpawnPoint->CachedSpawnCandidateClasses.IsEmpty())
		{
			TWeakObjectPtr<ARPSpawnPoint> RepresentSpawnPoint = AllSpawnPointIds.FindRef(SpawnPoint->SpawnPointId);
			if (RepresentSpawnPoint.IsValid())
			{
				SpawnPoint->CachedSpawnCandidateClasses = RepresentSpawnPoint.Get()->CachedSpawnCandidateClasses;
			}
			else
			{
				// SpawnPoint의 태그
				for (const FGameplayTag& SpawnPointTag : SpawnPoint->SpawnGroupTags)
				{
					if (!SpawnPointTag.IsValid())
					{
						continue;
					}

					// AssetManager에 있는 전체 SpawnGroup들의 태그 : (Group->GroupTag)
					for (const URPSpawnGroupAsset* Group : AllGroups)
					{
						if (!Group)
						{
							continue;
						}

						// 태그 비교(부모 태그랑도 비교할 것인지 체크)
						const bool bMatch = bMatchParentTags ? (SpawnPointTag.MatchesTag(Group->GroupTag) || Group->GroupTag.MatchesTag(SpawnPointTag)) : (SpawnPointTag == Group->GroupTag);

						if (!bMatch)
						{
							continue;
						}

						for (const TSubclassOf<AActor>& SpawnClass : Group->SpawnCandidateClasses)
						{
							if (IsValid(SpawnClass))
							{
								SpawnPoint->CachedSpawnCandidateClasses.AddUnique(SpawnClass);
							}
						}
					}
				}

				AllSpawnPointIds.Add(SpawnPoint->SpawnPointId, SpawnPoint);
			}
		}

		PointCounts.Add(SpawnPoint, 0);

		// 스폰(항상 최대 개수를 스폰하는 것은 아님)
		for (int i = 0; i < SpawnPoint->MaxSpawnCount; i++)
		{
			if (SpawnFromPoint(SpawnPoint, Stream))
			{
				PointCounts.FindOrAdd(SpawnPoint)++;
			}
		}
	}

	bIsAlreadySpawned = true;
}

bool ARPInteractableObjectSpawnManager::SpawnFromPoint(ARPSpawnPoint* Point, FRandomStream& Stream)
{
	const TArray<TSubclassOf<AActor>>& SpawnPool = Point->CachedSpawnCandidateClasses;
	if (SpawnPool.IsEmpty())
	{
		return false;
	}

	// 스폰 액터 클래스 선택 : 동일 확률 선택
	const int32 Pick = Stream.RandRange(0, SpawnPool.Num() - 1);
	UClass* SpawnActorClass = SpawnPool[Pick].Get();
	if (!SpawnActorClass)
	{
		return false;
	}

	// 스폰 위치
	FTransform SpawnTransform;
	if (!FindSpawnTransform(Point, SpawnTransform))
	{
		return false;
	}

	// 스폰 Param : Owner 설정
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	//AActor* SpawnActor = GetWorld()->SpawnActor<AActor>(SpawnActorClass, SpawnTransform, Params);
	//if (!SpawnActor)
	//{
	//	return false;
	//}

	AActor* SpawnActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnActorClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (!SpawnActor)
	{
		return false;
	}
	ARPBaseInteractableObject* BaseObject = Cast<ARPBaseInteractableObject>(SpawnActor);
	if(!BaseObject)
	{
		return false;
	}

	// 스폰 액터가 Bug 오브젝트인지
	BaseObject->bIsBug = GetIsBugged();
	UGameplayStatics::FinishSpawningActor(SpawnActor, SpawnTransform);

	SpawnedToPoint.Add(SpawnActor, Point);

	// 파괴 델리게이트 바인딩
	SpawnActor->OnDestroyed.AddDynamic(this, &ARPInteractableObjectSpawnManager::HandleSpawnedActorDestroyed);

	return true;
}

bool ARPInteractableObjectSpawnManager::FindSpawnTransform(ARPSpawnPoint* Point, FTransform& Out) const
{
	FVector PointLocation = Point->GetActorLocation();
	FRotator PointRotation = Point->GetActorRotation();

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Point);
	Params.AddIgnoredActor(this);

	const FVector Start = PointLocation + FVector(0, 0, 150);
	const FVector End = PointLocation - FVector(0, 0, 200);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		PointLocation = Hit.ImpactPoint;
	}

	// 스폰 자리에서 충돌 체크
	//const bool bOverlap = GetWorld()->OverlapAnyTestByChannel(PointLocation, FQuat::Identity, OverlapChannel, FCollisionShape::MakeSphere(Point->Radius), Params);
	//if (bOverlap)
	//{
	//	return false;
	//}

	Out = FTransform(PointRotation, PointLocation);

	return true;
}

bool ARPInteractableObjectSpawnManager::GetIsBugged() const
{
	int bug = FMath::RandRange(1, 100);

	if (bug > 50)
	{
		return false;
	}

	return true;
}

void ARPInteractableObjectSpawnManager::HandleSpawnedActorDestroyed(AActor* DestroyedActor)
{
	if (!HasAuthority())
	{
		return;
	}

	if (TWeakObjectPtr<ARPSpawnPoint>* SpawnPoint = SpawnedToPoint.Find(DestroyedActor))
	{
		PointCounts[*SpawnPoint] = FMath::Max(0, PointCounts[*SpawnPoint] - 1);
		SpawnedToPoint.Remove(DestroyedActor);
	}
}
