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

	if (IsValid(InventoryWidgetClass))
	{
		InventoryWidget = CreateWidget<UInventoryWidget>(Cast<APlayerController>(GetController()), InventoryWidgetClass);
		InventoryWidget->AddToViewport(0);
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
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
