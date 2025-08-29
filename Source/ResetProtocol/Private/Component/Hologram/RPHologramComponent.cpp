// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/Hologram/RPHologramComponent.h"
#include "Component/Hologram/RPPlacementSlot.h"
#include "InteractableObject/RPBaseInteractableObject.h"
#include "Kismet/GameplayStatics.h"

URPHologramComponent::URPHologramComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(false);
}

void URPHologramComponent::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle RefreshTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(RefreshTimerHandle, this, &URPHologramComponent::RefreshSlots, 2.0f, false);
}

void URPHologramComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void URPHologramComponent::ActivateHologram()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	if (CachedAllSlots.IsEmpty() || MatchingSlots.IsEmpty())
	{
		RefreshSlots();
	}

	// 제자리에 놓인 물체가 있을 수 있으니 MatchingSlots를 갱신
	FindAllMatchingSlots();

	for (const TWeakObjectPtr<ARPPlacementSlot> Weak_Slot : MatchingSlots)
	{
		if (Weak_Slot.IsValid())
		{
			Weak_Slot.Get()->ShowHologramFromActor(Owner);
		}
	}
}

void URPHologramComponent::DeactivateHologram()
{
	for (const TWeakObjectPtr<ARPPlacementSlot> Weak_Slot : MatchingSlots)
	{
		if (Weak_Slot.IsValid())
		{
			Weak_Slot.Get()->HideHologram();
		}
	}

	ClosestSlot = nullptr;
}

bool URPHologramComponent::ConfirmInPlace(FTransform& OutTransform)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	if (CachedAllSlots.IsEmpty() || MatchingSlots.IsEmpty())
	{
		RefreshSlots();
	}
	PickClosestSlot();

	float Score = TNumericLimits<float>::Max();
	if (ClosestSlot.IsValid())
	{
		if (ClosestSlot->IsInPlaceRangeAndScore(Owner, Owner->GetActorLocation(), Score))
		{
			if (Score < 100.0f)
			{
				ClosestSlot->TryGetSnapTransform(Owner, OutTransform);
				return true;
			}
		}
	}

	return false;
}

void URPHologramComponent::UpdateSlotOccupation()
{
	ClosestSlot->bIsOccupied = true;
}

void URPHologramComponent::RefreshSlots()
{
	CachedAllSlots.Reset();

	TArray<AActor*> Slots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARPPlacementSlot::StaticClass(), Slots);

	CachedAllSlots.Reserve(Slots.Num());
	for (AActor* Slot : Slots)
	{
		if (ARPPlacementSlot* PlacementSlot = Cast<ARPPlacementSlot>(Slot))
		{
			CachedAllSlots.Add(PlacementSlot);
		}
	}

	FindAllMatchingSlots();
}

void URPHologramComponent::FindAllMatchingSlots()
{
	MatchingSlots.Reset();

	AActor* Owner = GetOwner();

	const ARPBaseInteractableObject* BaseObject = Cast<ARPBaseInteractableObject>(Owner);
	if (!BaseObject)
	{
		return;
	}
	if (!BaseObject->HologramSlotTag.IsValid())
	{
		return;
	}

	for (TWeakObjectPtr<ARPPlacementSlot> Weak_Slot : CachedAllSlots)
	{
		if (ARPPlacementSlot* Slot = Weak_Slot.Get())
		{
			if (Slot->IsCompatibleForActor(BaseObject) && !Slot->bIsOccupied)
			{
				MatchingSlots.Add(Slot);
			}
		}
	}
}

void URPHologramComponent::PickClosestSlot()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	FVector DropLocation = Owner->GetActorLocation();
	double MinDistance = TNumericLimits<double>::Max();

	for (const TWeakObjectPtr<ARPPlacementSlot> Slot : MatchingSlots)
	{
		if (Slot.IsValid() && !Slot->bIsOccupied)
		{
			double Distance = FVector::Distance(DropLocation, Slot->GetActorLocation());

			if (Distance < MinDistance)
			{
				ClosestSlot = Slot.Get();
				MinDistance = Distance;
			}
		}
	}
	
	return;
}