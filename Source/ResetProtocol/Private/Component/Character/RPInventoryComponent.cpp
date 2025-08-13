#include "Component/Character/RPInventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"

#include "Character/RPPlayerCharacter.h"
#include "UI/Inventory/InventoryWidget.h"
#include "RPTestItemActor.h"

URPInventoryComponent::URPInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void URPInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void URPInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void URPInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPInventoryComponent, Inventory);
}

void URPInventoryComponent::ToggleInventory(AController* Controller)
{
	if (IsValid(InventoryWidget) && IsValid(Controller))
	{
		if (!InventoryWidget->IsVisible())
		{
			InventoryWidget->SetVisibility(ESlateVisibility::Visible);
			InventoryWidget->RefreshInventory(Inventory);

			Cast<APlayerController>(Controller)->SetInputMode(FInputModeGameAndUI());
			Cast<APlayerController>(Controller)->SetCinematicMode(true, true, true);
			Cast<APlayerController>(Controller)->bShowMouseCursor = true;
		}
		else
		{
			InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);

			Cast<APlayerController>(Controller)->SetInputMode(FInputModeGameOnly());
			Cast<APlayerController>(Controller)->SetCinematicMode(false, true, true);
			Cast<APlayerController>(Controller)->bShowMouseCursor = false;
		}
	}
}

void URPInventoryComponent::CreateInventoryWidget(AController* Controller)
{
	if (IsValid(InventoryWidgetClass))
	{
		InventoryWidget = CreateWidget<UInventoryWidget>(Cast<APlayerController>(Controller), InventoryWidgetClass);

		if (IsValid(InventoryWidget))
		{
			InventoryWidget->AddToViewport(10);
			InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

bool URPInventoryComponent::Server_DropItem_Validate(const FItemData& DroppedItem)
{
	return DroppedItem.Class != nullptr;
}

void URPInventoryComponent::Server_DropItem_Implementation(const FItemData& DroppedItem)
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	if (IsValid(DroppedItem.Class) && IsValid(PlayerCharacter))
	{
		GetWorld()->SpawnActor<ARPTestItemActor>(DroppedItem.Class, PlayerCharacter->GetInteractorComponent()->GetInteractEnd(), FRotator::ZeroRotator);
	}
}

void URPInventoryComponent::OnRep_Inventory()
{
	if (IsValid(InventoryWidget))
	{
		InventoryWidget->RefreshInventory(Inventory);
	}
}