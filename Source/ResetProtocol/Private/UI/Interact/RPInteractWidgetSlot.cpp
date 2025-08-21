#include "UI/Interact/RPInteractWidgetSlot.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"

#include "Data/RPInteractUIData.h"


void URPInteractWidgetSlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);


	UInteractDataUIObject* UIItem = Cast<UInteractDataUIObject>(ListItemObject);
	InputText->SetText(FText::FromString(UIItem->ItemData.InputText));
	
	InteractText->SetText(FText::FromString(UIItem->ItemData.InteractText));

	IconImage->SetBrushFromTexture(UIItem->ItemData.Icon);
}
