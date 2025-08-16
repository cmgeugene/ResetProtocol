// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Interact/RPInteractWidget.h"
#include "Components/TextBlock.h"

void URPInteractWidget::SetText(const FString& string)
{
	InteractText->SetText(FText::FromString(string));
}