#include "UI/Inventory/RPHotbarWidget.h"
#include "Components/UniformGridPanel.h"

#include "Character/RPPlayerCharacter.h"
#include "RPTestItemActor.h"
#include "CleaningTool/RPBaseCleaningTool.h"


URPHotbarWidget::URPHotbarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, SlotIndex(-1)
	, LatestIndex(0)
	, IsFull(false)
{
}

void URPHotbarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateUI();
}

void URPHotbarWidget::UpdateUI(int Index)
{
	APlayerController* PlayerController = GetOwningPlayer();

	if (IsValid(PlayerController))
	{
		ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(PlayerController->GetCharacter());

		if (IsValid(PlayerCharacter))
		{
			TArray<FCleaningToolData>& Inventory = PlayerCharacter->GetHotbarComponent()->GetInventory();

			UGridPanel_Items->ClearChildren();

			SlotIndex = -1;
			LatestIndex = 0;

			for (FCleaningToolData Item : Inventory)
			{
				SlotIndex++;

				URPHotbarSlot* NewHotbarSlot = CreateWidget<URPHotbarSlot>(this, HotbarSlotClass);
				NewHotbarSlot->SetData(Item, SlotIndex);

				if (IsValid(Item.Class))
				{
					if(LatestIndex == SlotIndex)
						LatestIndex = SlotIndex + 1;
				}

				if (SlotIndex < 2)
				{
					UGridPanel_Items->AddChildToUniformGrid(NewHotbarSlot, 0, SlotIndex);
				}
				else
				{
					UGridPanel_Items->AddChildToUniformGrid(NewHotbarSlot, 1, SlotIndex - 2);
				}
			}

			if (LatestIndex >= Inventory.Num())
			{
				IsFull = true;
			}
			else
			{
				IsFull = false;
			}

		}
	}

	
}

int URPHotbarWidget::GetLatestIndex()
{
	if (IsFull)
		return -1;
	else
		return LatestIndex;

	//return 0;
}

void URPHotbarWidget::OnHighlight(int CurrentIndex, int PreviousIndex)
{
	URPHotbarSlot* CurrentHotbarSlot = Cast<URPHotbarSlot>(UGridPanel_Items->GetChildAt(CurrentIndex));
	URPHotbarSlot* PreviousHotbarSlot = Cast<URPHotbarSlot>(UGridPanel_Items->GetChildAt(PreviousIndex));

	if (IsValid(CurrentHotbarSlot))
	{
		if (IsValid(PreviousHotbarSlot))
		{
			PreviousHotbarSlot->HighlightOff();
		}

		CurrentHotbarSlot->HighlightOn();
	}
}

void URPHotbarWidget::OffHighlight(int PreviousIndex)
{
	URPHotbarSlot* PreviousHotbarSlot = Cast<URPHotbarSlot>(UGridPanel_Items->GetChildAt(PreviousIndex));

	if (IsValid(PreviousHotbarSlot))
	{
		PreviousHotbarSlot->HighlightOff();
	}
}
