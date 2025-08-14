#include "UI/Inventory/RPHotbarSlot.h"
#include "Styling/SlateBrush.h"

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
	FSlateBrush Brush = Highlight->Brush;
	Brush.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 0.f, 0.25f)); 
	Highlight->SetBrush(Brush);
}

void URPHotbarSlot::HighlightOff()
{
	FSlateBrush Brush = Highlight->Brush;
	Brush.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 0.f, 0.f));
	Highlight->SetBrush(Brush);
}
