#include "Component/RPInteractorComponent.h"
#include "Character/RPPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Data/ItemDataBase.h"
#include "RPTestItemActor.h"

URPInteractorComponent::URPInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void URPInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InteractWidgetClass))
	{
		ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());
		if (IsValid(PlayerCharacter))
		{
			InteractWidget = CreateWidget(Cast<APlayerController>(PlayerCharacter->GetController()), InteractWidgetClass);

			if (IsValid(InteractWidget))
			{
				InteractWidget->AddToViewport(0);
				InteractWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}


void URPInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	InteractCheck();
}

void URPInteractorComponent::Interact()
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());
	
	if (IsValid(PlayerCharacter))
	{
		ARPTestItemActor* TestItemActor = Cast<ARPTestItemActor>(PlayerCharacter->GetHitResult().GetActor());
		if (IsValid(TestItemActor))
		{
			FItemData* Data = PlayerCharacter->GetItemDataBase()->Items.FindByPredicate([&](const FItemData& ItemData)
				{
					//return ItemData.Class == PlayerCharacter->GetHitResult().GetActor()->GetClass();
					return ItemData.Class == TestItemActor->GetClass();
				});

			if (Data != nullptr)
			{
				PlayerCharacter->GetInventory().Emplace(*Data);
				TestItemActor->Destroy();
			}


			if (TestItemActor->GetMaterialType() == EMaterialState::Sphere)
			{
				UE_LOG(LogTemp, Warning, TEXT("Sphere"));
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Sphere"));
			}
			else if (TestItemActor->GetMaterialType() == EMaterialState::Con)
			{
				UE_LOG(LogTemp, Warning, TEXT("Con"));
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Con"));
			}
			else if (TestItemActor->GetMaterialType() == EMaterialState::Cube)
			{
				UE_LOG(LogTemp, Warning, TEXT("Cube"));				
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Cube"));
			}
		}
	}
}

void URPInteractorComponent::InteractCheck()
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	if (IsValid(PlayerCharacter))
	{
		Cast<APlayerController>(PlayerCharacter->GetController())->GetPlayerViewPoint(ViewVector, ViewRotation);
	}

	FVector VecDirection = ViewRotation.Vector() * InteractionRange;
	InteractEnd = ViewVector + VecDirection;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(PlayerCharacter);
	GetWorld()->LineTraceSingleByChannel(PlayerCharacter->GetHitResult(), ViewVector, InteractEnd, ECollisionChannel::ECC_GameTraceChannel1, QueryParams);

	if (IsValid(InteractWidget))
	{
		ARPTestItemActor* TestItemActor = Cast<ARPTestItemActor>(PlayerCharacter->GetHitResult().GetActor());
		if (TestItemActor)
		{
			InteractWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			InteractWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}