#include "Character/RPPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
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
}

void ARPPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARPPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
