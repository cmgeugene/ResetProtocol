// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPHologramComponent.generated.h"

class ARPPlacementSlot;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESETPROTOCOL_API URPHologramComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URPHologramComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ActivateHologram();
	void DeactivateHologram();

	bool ConfirmInPlace(FTransform& OutTransform);

	void UpdateSlotOccupation();
	
protected:
	virtual void BeginPlay() override;

private:
	void RefreshSlots();
	void FindAllMatchingSlots();
	void PickClosestSlot();

private:
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<ARPPlacementSlot>> CachedAllSlots;
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<ARPPlacementSlot>> MatchingSlots;
	UPROPERTY(Transient)
	TWeakObjectPtr<ARPPlacementSlot> ClosestSlot;

	// GrabbedActor도 가지고 있어도 될 것 같은데 => Owner로 처리하면됨
};
