// Fill out your copyright notice in the Description page of Project Settings.

#include "Manager/InteractableObject/RPInteractableObjectSpawnManager.h"
#include "Manager/InteractableObject/RPSpawnGroupAsset.h"
#include "Manager/InteractableObject/RPSpawnPoint.h"
#include "InteractableObject/RPCorpse.h"
#include "InteractableObject/RPBaseInteractableObject.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ARPInteractableObjectSpawnManager::ARPInteractableObjectSpawnManager() :
	bIsSpawnAtBeginPlay(true),
	InitDelay(0.1f),
	RandomSeed(4885),
	bMatchParentTags(false),
	//OverlapChannel(ECC_Pawn),
	BugProbability(20),
	bIsAlreadySpawned(false),
	NumOfObjects(0)
{
	bReplicates = true;
	SetReplicateMovement(false);
	PrimaryActorTick.bCanEverTick = false;
}

void ARPInteractableObjectSpawnManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPInteractableObjectSpawnManager, NumOfObjects);
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
			UE_LOG(LogTemp, Warning, TEXT("Tag Empty : %s"), *SpawnPoint->SpawnPointId.ToString());
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
						UE_LOG(LogTemp, Warning, TEXT("Tag is Invalid : %s"), *SpawnPoint->SpawnPointId.ToString());
						continue;
					}

					// AssetManager에 있는 전체 SpawnGroup들의 태그 : (Group->GroupTag)
					for (const URPSpawnGroupAsset* Group : AllGroups)
					{
						if (!Group)
						{
							UE_LOG(LogTemp, Warning, TEXT("Asset Manager doesn't have Group : %s"), *SpawnPoint->SpawnPointId.ToString());
							continue;
						}

						// 태그 비교(부모 태그랑도 비교할 것인지 체크)
						const bool bMatch = bMatchParentTags ? (SpawnPointTag.MatchesTag(Group->GroupTag) || Group->GroupTag.MatchesTag(SpawnPointTag)) : (SpawnPointTag == Group->GroupTag);

						if (!bMatch)
						{
							UE_LOG(LogTemp, Warning, TEXT("There is no matching tags / Point Tag = %s, Manager Tag = %s"), *SpawnPointTag.GetTagName().ToString(), *Group->GroupTag.GetTagName().ToString());
							continue;
						}

						for (const TSubclassOf<AActor>& SpawnClass : Group->SpawnCandidateClasses)
						{
							if (IsValid(SpawnClass))
							{
								UE_LOG(LogTemp, Warning, TEXT("Add to Candidate : %s"), *SpawnClass->StaticClass()->GetName());
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

	TArray<AActor*> AllObjects;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARPBaseInteractableObject::StaticClass(), AllObjects);

	NumOfObjects = AllObjects.Num();

	UE_LOG(LogTemp, Warning, TEXT("Spawn : %d / All : %d"), SpawnedToPoint.Num(), NumOfObjects);
}

bool ARPInteractableObjectSpawnManager::SpawnFromPoint(ARPSpawnPoint* Point, FRandomStream& Stream)
{
	const TArray<TSubclassOf<AActor>>& SpawnPool = Point->CachedSpawnCandidateClasses;
	if (SpawnPool.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Spawn Fail : %s"), *Point->SpawnPointId.ToString()));
		return false;
	}

	// 스폰 액터 클래스 선택 : 동일 확률 선택
	const int32 Pick = Stream.RandRange(0, SpawnPool.Num() - 1);
	UClass* SpawnActorClass = SpawnPool[Pick].Get();
	if (!SpawnActorClass)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Spawn Fail : %s"), *Point->SpawnPointId.ToString()));
		return false;
	}

	// 스폰 위치
	FTransform SpawnTransform;
	if (!FindSpawnTransform(Point, SpawnActorClass, SpawnTransform))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Spawn Fail : %s"), *Point->SpawnPointId.ToString()));
		return false;
	}

	// 스폰 Param : Owner 설정
	//FActorSpawnParameters Params;
	//Params.Owner = this;
	//Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//
	//AActor* SpawnActor = GetWorld()->SpawnActor<AActor>(SpawnActorClass, SpawnTransform, Params);
	//if (!SpawnActor)
	//{
	//	return false;
	//}

	AActor* SpawnActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnActorClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (!SpawnActor)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Spawn Fail : %s"), *Point->SpawnPointId.ToString()));
		return false;
	}
	ARPBaseInteractableObject* BaseObject = Cast<ARPBaseInteractableObject>(SpawnActor);
	if(!BaseObject)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Spawn Fail : %s"), *Point->SpawnPointId.ToString()));
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

bool ARPInteractableObjectSpawnManager::FindSpawnTransform(ARPSpawnPoint* Point, UClass* SpawnActorClass, FTransform& Out) const
{
	FVector PointLocation = Point->GetActorLocation();
	FRotator PointRotation = Point->GetActorRotation();

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Point);
	Params.AddIgnoredActor(this);

	const FVector Start = PointLocation + FVector(0, 0, 50);
	const FVector End = PointLocation - FVector(0, 0, 100);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		PointLocation = Hit.ImpactPoint;
		if (SpawnActorClass == ARPCorpse::StaticClass())
		{
			PointLocation += FVector(0, 0, 40);
		}
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
	if (FMath::RandRange(1, 100) > BugProbability)
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

	UE_LOG(LogTemp, Warning, TEXT("[SpawnDbg] Destroyed %s at %s (Age? N/A)"),
		*GetNameSafe(DestroyedActor),
		*DestroyedActor->GetActorLocation().ToString());
}
