#include "Component/Character/RPHotbarComponent.h"
#include "Net/UnrealNetwork.h"

#include "Character/RPPlayerCharacter.h"
#include "UI/Inventory/RPHotbarWidget.h"
#include "RPTestItemActor.h"

URPHotbarComponent::URPHotbarComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;

	TotalItems = 4;
	CurrentSlotIndex = -1;
}


void URPHotbarComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		for (int i = 0; i < TotalItems; i++)
		{
			Inventory.Add(FItemData());
		}
	}
}


void URPHotbarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void URPHotbarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPHotbarComponent, Inventory);
}

void URPHotbarComponent::CreateHotbarWidget_Implementation(AController* Controller)
{
	if (IsValid(HotbarWidgetClass))
	{
		HotbarWidget = CreateWidget<URPHotbarWidget>(Cast<APlayerController>(Controller), HotbarWidgetClass);

		if (IsValid(HotbarWidget))
		{
			HotbarWidget->AddToViewport(0);
			HotbarWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

bool URPHotbarComponent::Server_DropItem_Validate(const FItemData& DroppedItem)
{
	return DroppedItem.Class != nullptr;
}

void URPHotbarComponent::Server_DropItem_Implementation(const FItemData& DroppedItem)
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	if (IsValid(DroppedItem.Class) && IsValid(PlayerCharacter))
	{
		GetWorld()->SpawnActor<ARPTestItemActor>(DroppedItem.Class, PlayerCharacter->GetInteractorComponent()->GetInteractEnd(), FRotator::ZeroRotator);
	}
}
void URPHotbarComponent::OnRep_Inventory()
{
	if (IsValid(HotbarWidget))
	{
		HotbarWidget->UpdateUI();
	}
}

void URPHotbarComponent::SelectItem(int SelectedNum)
{
	HotbarWidget->OnHighlight(SelectedNum, CurrentSlotIndex);

	CurrentSlotIndex = SelectedNum;
}

void URPHotbarComponent::UnEquip()
{
	HotbarWidget->OffHighlight(CurrentSlotIndex);

	CurrentSlotIndex = -1;
}

void URPHotbarComponent::AddItem_Implementation(const FItemData& Data)
{
	int LatestIndex = HotbarWidget->GetLatestIndex();

	if (LatestIndex != -1)
	{
		Inventory[LatestIndex] = Data;
		HotbarWidget->UpdateUI();
	}
	else
	{
		return;
	}
}
