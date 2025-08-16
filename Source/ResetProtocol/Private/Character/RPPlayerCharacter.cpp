#include "Character/RPPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

#include "UI/Inventory/InventoryWidget.h"
#include "RPTestItemActor.h"


ARPPlayerCharacter::ARPPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	InteractorComponent = CreateDefaultSubobject<URPInteractorComponent>(TEXT("InteractorComponent"));
	InventoryComponent = CreateDefaultSubobject<URPInventoryComponent>(TEXT("InventoryComponent"));
	HotbarComponent = CreateDefaultSubobject<URPHotbarComponent>(TEXT("HotbarComponent"));

	GrabComponent = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("GrabComponent"));
}

void ARPPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (IsValid(PlayerController) && PlayerController->IsLocalController())
	{
		InteractorComponent->CreateInteractWidget(GetController());
		InventoryComponent->CreateInventoryWidget(GetController());
		HotbarComponent->CreateHotbarWidget(GetController());
	}
}

void ARPPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARPPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("LeftMouseButton", EInputEvent::IE_Released, this, &ARPPlayerCharacter::OnLeftMouseButtonReleased);

}

void ARPPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ARPPlayerCharacter::OnLeftMouseButtonReleased()
{
	InteractorComponent->OnLeftMouseButtonReleased();
}