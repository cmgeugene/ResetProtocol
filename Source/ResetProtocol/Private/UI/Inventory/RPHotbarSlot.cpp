#include "UI/Inventory/RPHotbarSlot.h"

URPHotbarSlot::URPHotbarSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URPHotbarSlot::NativeConstruct()
{
	Super::NativeConstruct();

}

void URPHotbarSlot::SetData(const FItemData& _ItemData, int _SlotIndexs)
{
	ItemData = _ItemData;
	SlotIndexs = _SlotIndexs;

	if (IsValid(_ItemData.Icon))
	{
		IconImage->SetBrushFromTexture(_ItemData.Icon);
		IconImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));

	}
	else
	{
		IconImage->SetBrushFromTexture(nullptr);
		IconImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));

	}

	Text_KeyIndex->SetText(FText::AsNumber(SlotIndexs));
}

void URPHotbarSlot::HighlightOn()
{
	Highlight->SetColorAndOpacity(FLinearColor(1.f, 1.f, 0.f, 025.f));
}

void URPHotbarSlot::HighlightOff()
{
	Highlight->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));
}
