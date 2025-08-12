#include "Character/RPPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "UI/Inventory/InventoryWidget.h"
#include "RPTestItemActor.h"
#include "Component/RPInteractorComponent.h"

ARPPlayerCharacter::ARPPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractorComponent = CreateDefaultSubobject<URPInteractorComponent>(TEXT("InteractorComponent"));

}

void ARPPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (IsValid(PlayerController) && PlayerController->IsLocalController())
	{
		if (IsValid(InventoryWidgetClass))
		{
			InventoryWidget = CreateWidget<UInventoryWidget>(Cast<APlayerController>(GetController()), InventoryWidgetClass);
			InventoryWidget->AddToViewport(0);
			InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

		InteractorComponent->CreateInteractWidget(GetController());
	}
}

void ARPPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARPPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ARPPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPPlayerCharacter, Inventory);
}

void ARPPlayerCharacter::ToggleInventory()
{
	if (IsValid(InventoryWidget))
	{
		if (!InventoryWidget->IsVisible())
		{
			InventoryWidget->SetVisibility(ESlateVisibility::Visible);
			InventoryWidget->RefreshInventory(Inventory);

			Cast<APlayerController>(GetController())->SetInputMode(FInputModeGameAndUI());
			Cast<APlayerController>(GetController())->SetCinematicMode(true, true, true);
			Cast<APlayerController>(GetController())->bShowMouseCursor = true;
		}
		else
		{
			InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);

			Cast<APlayerController>(GetController())->SetInputMode(FInputModeGameOnly());
			Cast<APlayerController>(GetController())->SetCinematicMode(false, true, true);
			Cast<APlayerController>(GetController())->bShowMouseCursor = false;
		}
	}
}

void ARPPlayerCharacter::OnRep_Inventory()
{
	if (IsValid(InventoryWidget))
	{
		InventoryWidget->RefreshInventory(Inventory);
	}
}

bool ARPPlayerCharacter::Server_DropItem_Validate(const FItemData& DroppedItem, FVector SpawnLocation)
{
	return DroppedItem.Class != nullptr;
}

void ARPPlayerCharacter::Server_DropItem_Implementation(const FItemData& DroppedItem, FVector SpawnLocation)
{
	if (IsValid(DroppedItem.Class))
	{
		GetWorld()->SpawnActor<ARPTestItemActor>(DroppedItem.Class, GetInteractEnd(), FRotator::ZeroRotator);
	}
}
