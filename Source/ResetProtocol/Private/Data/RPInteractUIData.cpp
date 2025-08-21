// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/RPInteractUIData.h"

FInteractUIData URPInteractUIData::FindItem(const FName& ItemNamd)
{
	return *Items.FindByKey(ItemNamd);
}
