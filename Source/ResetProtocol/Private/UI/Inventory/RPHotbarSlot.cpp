#include "UI/Inventory/RPHotbarSlot.h"
#include "Styling/SlateBrush.h"
#include "CleaningTool/RPBaseCleaningTool.h"


URPHotbarSlot::URPHotbarSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URPHotbarSlot::NativeConstruct()
{
	Super::NativeConstruct();

}

void URPHotbarSlot::SetData(const FCleaningToolData& _ItemData, int _SlotIndexs)
{
	ItemData = _ItemData;
	SlotIndexs = _SlotIndexs;

	if (IsValid(_ItemData.Class))
	{
		IconImage->SetBrushFromTexture(_ItemData.Icon);
		IconImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));

		float Percent = (float)_ItemData.CurrentDurability / (float)_ItemData.Durability;
		DurabilityBar->SetPercent(Percent);
		DurabilityBar->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		IconImage->SetBrushFromTexture(nullptr);
		IconImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));
		DurabilityBar->SetVisibility(ESlateVisibility::Hidden);
	}



	Text_KeyIndex->SetText(FText::AsNumber(SlotIndexs + 1));
	
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
