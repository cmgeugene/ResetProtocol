#include "Character/RPPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Net/UnrealNetwork.h"

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
		HotbarComponent->Client_CreateHotbarWidget(GetController());
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

	DOREPLIFETIME(ARPPlayerCharacter, PlayerInfo);
}

bool ARPPlayerCharacter::SpendBytes(int Bytes)
{
	if (PlayerInfo.Bytes < Bytes)
	{
		return false;
	}
	else
	{
		PlayerInfo.Bytes -= Bytes;

		Server_UpdatePlayerInfo();
		return true;
	}
}

void ARPPlayerCharacter::OnLeftMouseButtonReleased()
{
	InteractorComponent->OnLeftMouseButtonReleased();
}

void ARPPlayerCharacter::OnRep_PlayerInfo()
{
	BP_OnPlayerInfoChanged();
}

void ARPPlayerCharacter::Server_UpdatePlayerInfo_Implementation()
{
	if (HasAuthority())
	{
		BP_OnPlayerInfoChanged();
	}
}


