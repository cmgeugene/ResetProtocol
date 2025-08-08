#include "UI/Inventory/InventoryEntry.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "UI/Inventory/InventoryWidget.h"
#include "Character/RPPlayerCharacter.h"
#include "RPTestItemActor.h"
#include "Data/ItemDataBase.h"

void UInventoryEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UItemUIObject* UIItem = Cast<UItemUIObject>(ListItemObject);
	ItemName->SetText(FText::FromString(UIItem->ItemData.ItemName.ToString()));

	DropButton->OnClicked.Clear();
	DropButton->OnClicked.AddDynamic(this, &UInventoryEntry::DropItem);
}

void UInventoryEntry::DropItem()
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwningPlayer()->GetPawn());
	
	FItemData ItemData = GetListItem<UItemUIObject>()->ItemData;

	GetWorld()->SpawnActor<ARPTestItemActor>(ItemData.Class, PlayerCharacter->GetInteractEnd(), FRotator());
	PlayerCharacter->GetInventory().Remove(ItemData);
	PlayerCharacter->GetInventoryWidget()->RefreshInventory(PlayerCharacter->GetInventory());
	DropButton->OnClicked.Clear();
}
