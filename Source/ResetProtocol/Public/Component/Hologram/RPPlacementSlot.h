// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "RPPlacementSlot.generated.h"

class UArrowComponent;

UCLASS()
class RESETPROTOCOL_API ARPPlacementSlot : public AActor
{
	GENERATED_BODY()
	
public:
	ARPPlacementSlot();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Grab한 액터를 이 Slot에 놓을 수 있는 지
	bool IsCompatibleForActor(const AActor* Actor) const;

	void ShowHologramFromActor(const AActor* GrabbedActor);
	void HideHologram();

	// 겹침 판정 Score : 작을 수록 좋음
	bool IsInPlaceRangeAndScore(const AActor* Actor, const FVector& DropLocation, float& OutScore) const;

	bool TryGetSnapTransform(const AActor* Actor, FTransform& OutTransform) const;

protected:
	virtual void BeginPlay() override;

private:
	UStaticMeshComponent* FindActorStaticMesh(const AActor* Actor) const;
	USkeletalMeshComponent* FindActorSkeletalMesh(const AActor* Actor) const;
	// 메시들의 바닥이 slot 위치로 오도록
	void StaticMeshLocationAligned();
	void SkeletalMeshLocationAligned();
	void ApplyGhostMaterial(UMeshComponent* Mesh);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Placement|Components")
	TObjectPtr<USceneComponent> RootPlacePoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Placement|Components")
	TObjectPtr<UArrowComponent> PlaceArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Placement|Hologram")
	TObjectPtr<UStaticMeshComponent> StaticHologram;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Placement|Hologram")
	TObjectPtr<USkeletalMeshComponent> SkeletalHologram;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placement|Hologram")
	TObjectPtr<UMaterialInterface> HologramMaterial;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Placement|Tag")
	FGameplayTag SlotTag;

	UPROPERTY(Replicated)
	bool bIsOccupied;

	// Snap 오차 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placement|Range", meta = (ClampMin = "0.0"))
	float CaptureRadius;				// 수평
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placement|Range", meta = (ClampMin = "0.0"))
	float HeightTolerance;				// 수직
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placement|Align")
	float YawAlignToleranceDegree;		// 회전
};
