#include "Component/Character/RPInteractorComponent.h"
#include "Character/RPPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Data/ItemDataBase.h"
#include "RPTestItemActor.h"
#include "Component/Character/RPHotbarComponent.h"

URPInteractorComponent::URPInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void URPInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	//if (IsValid(InteractWidgetClass))
	//{
	//	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());
	//	if (IsValid(PlayerCharacter))
	//	{
	//		InteractWidget = CreateWidget(Cast<APlayerController>(PlayerCharacter->GetController()), InteractWidgetClass);

	//		if (IsValid(InteractWidget))
	//		{
	//			InteractWidget->AddToViewport(0);
	//			InteractWidget->SetVisibility(ESlateVisibility::Collapsed);
	//		}
	//	}
	//}
}

void URPInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	InteractCheck();
}

void URPInteractorComponent::CreateInteractWidget(AController* Controller)
{
	if (IsValid(InteractWidgetClass))
	{
		InteractWidget = CreateWidget(Cast<APlayerController>(Controller), InteractWidgetClass);

		if (IsValid(InteractWidget))
		{
			InteractWidget->AddToViewport(0);
			InteractWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}


bool URPInteractorComponent::Server_Interact_Validate(ARPTestItemActor* TargetActor)
{
	return IsValid(TargetActor);
}


void URPInteractorComponent::Server_Interact_Implementation(ARPTestItemActor* TargetActor)
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());
	if (IsValid(PlayerCharacter) && IsValid(TargetActor))
	{
		FItemData* Data = PlayerCharacter->GetHotbarComponent()->GetItemDataBase()->Items.FindByPredicate([&](const FItemData& ItemData)
			{
				return ItemData.Class == TargetActor->GetClass();
			});

		if (Data != nullptr)
		{
			//PlayerCharacter->GetHotbarInventory().Emplace(*Data);
			PlayerCharacter->GetHotbarComponent()->AddItem(*Data);

			TargetActor->Destroy();
		}

		// 디버그 로그
		if (TargetActor->GetMaterialType() == EMaterialState::Sphere)
		{
			UE_LOG(LogTemp, Warning, TEXT("Sphere"));
		}
		else if (TargetActor->GetMaterialType() == EMaterialState::Con)
		{
			UE_LOG(LogTemp, Warning, TEXT("Con"));
		}
		else if (TargetActor->GetMaterialType() == EMaterialState::Cube)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cube"));
		}
	}
}

void URPInteractorComponent::Interact()
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	if (IsValid(PlayerCharacter))
	{
		ARPTestItemActor* TestItemActor = Cast<ARPTestItemActor>(PlayerCharacter->GetHitResult().GetActor());
		if (IsValid(TestItemActor))
		{
			Server_Interact(TestItemActor);
		}
	}
}

void URPInteractorComponent::InteractCheck()
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	if (IsValid(PlayerCharacter))
	{
		APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());

		if (IsValid(PlayerController))
		{
			PlayerController->GetPlayerViewPoint(ViewVector, ViewRotation);
		}
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