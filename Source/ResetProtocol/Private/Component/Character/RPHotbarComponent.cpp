#include "Component/Character/RPHotbarComponent.h"
#include "Net/UnrealNetwork.h"

#include "Character/RPPlayerCharacter.h"
#include "UI/Inventory/RPHotbarWidget.h"
#include "RPTestItemActor.h"
#include "CleaningTool/RPBaseCleaningTool.h"

URPHotbarComponent::URPHotbarComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;

	TotalItems = 4;
	CurrentSlotIndex = -1;
}


void URPHotbarComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		for (int i = 0; i < TotalItems; i++)
		{
			Inventory.Add(FCleaningToolData());
		}
	}
}


void URPHotbarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void URPHotbarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPHotbarComponent, Inventory);
}

void URPHotbarComponent::CreateHotbarWidget_Implementation(AController* Controller)
{
	if (IsValid(HotbarWidgetClass))
	{
		HotbarWidget = CreateWidget<URPHotbarWidget>(Cast<APlayerController>(Controller), HotbarWidgetClass);

		if (IsValid(HotbarWidget))
		{
			HotbarWidget->AddToViewport(0);
			HotbarWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

bool URPHotbarComponent::Server_DropItem_Validate(const FCleaningToolData& DroppedItem)
{
	return DroppedItem.Class != nullptr;
}

void URPHotbarComponent::Server_DropItem_Implementation(const FCleaningToolData& DroppedItem)
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	if (IsValid(DroppedItem.Class) && IsValid(PlayerCharacter))
	{
		GetWorld()->SpawnActor<ARPTestItemActor>(DroppedItem.Class, PlayerCharacter->GetInteractorComponent()->GetInteractEnd(), FRotator::ZeroRotator);
	}
}
void URPHotbarComponent::OnRep_Inventory()
{
	if (IsValid(HotbarWidget))
	{
		HotbarWidget->UpdateUI();
	}
}

void URPHotbarComponent::SelectItem(int SelectedNum)
{
	HotbarWidget->OnHighlight(SelectedNum, CurrentSlotIndex);

	if (IsValid(CurrentCleaningTool))
	{
		CurrentCleaningTool->Destroy();
	}

	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	if (IsValid(Inventory[SelectedNum].Class) && IsValid(PlayerCharacter))
	{
		CurrentCleaningTool = GetWorld()->SpawnActor<ARPBaseCleaningTool>(Inventory[SelectedNum].Class, PlayerCharacter->GetInteractorComponent()->GetInteractEnd(), FRotator::ZeroRotator);
		
		USkeletalMeshComponent* PlayerMesh = PlayerCharacter->GetMesh();
		if (IsValid(PlayerMesh))
		{
			FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
			CurrentCleaningTool->AttachToComponent(PlayerMesh, AttachRules, FName("CleaningToolSocket"));
		}
	}

	CurrentSlotIndex = SelectedNum;
}

void URPHotbarComponent::UnEquip()
{
	HotbarWidget->OffHighlight(CurrentSlotIndex);

	if (IsValid(CurrentCleaningTool))
	{
		CurrentCleaningTool->Destroy();
	}

	CurrentSlotIndex = -1;
}

void URPHotbarComponent::AddItem_Implementation(const FCleaningToolData& Data)
{
	HotbarWidget->UpdateUI();

	int LatestIndex = HotbarWidget->GetLatestIndex();

	if (CurrentSlotIndex == -1)
	{
		if (LatestIndex >= 0 && LatestIndex <= 2)
		{
			Inventory[LatestIndex] = Data;
			HotbarWidget->UpdateUI();
		}
	}
	else
	{
		if (!IsValid(CurrentCleaningTool))
		{
			Inventory[CurrentSlotIndex] = Data;
			HotbarWidget->UpdateUI();
		}
	}
	
}
