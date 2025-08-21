// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Interact/RPInteractWidget.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"
#include "Components/Border.h"

void URPInteractWidget::SetText(const FString& string)
{
	InteractText->SetText(FText::FromString(string));
}

void URPInteractWidget::RefreshWidget()
{
	InteractList->ClearListItems();
}

void URPInteractWidget::AddList(const TArray<FInteractUIData>& Datas)
{
	RefreshWidget();

	for (FInteractUIData ItemData : Datas)
	{
		UInteractDataUIObject* UIItem = NewObject<UInteractDataUIObject>();
		UIItem->ItemData = ItemData;
		InteractList->AddItem(UIItem);
	}
}

void URPInteractWidget::VisiblePrice(int Price)
{
	BytesText->SetText(FText::AsNumber(Price));

	FLinearColor NewColor(1.f, 1.f, 1.f, 1.0f);
	//PriceBorder->SetBrushColor(NewColor);
	PriceBorder->SetContentColorAndOpacity(NewColor);
}

void URPInteractWidget::InvisiblePrice()
{
	FLinearColor NewColor(1.f, 0.f, 0.f, 0.0f);
	PriceBorder->SetContentColorAndOpacity(NewColor);
}
