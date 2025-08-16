#include "Component/Character/RPInteractorComponent.h"
#include "Character/RPPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Data/ItemDataBase.h"
#include "RPTestItemActor.h"
#include "Component/Character/RPHotbarComponent.h"
#include "ResetProtocol/ResetProtocol.h"

#include "InteractableObject/RPBaseInteractableObject.h"

#include "Interface/RPClickInterface.h"
#include "Interface/RPDragInterface.h"
#include "Interface/RPKeyHoldInterface.h"

URPInteractorComponent::URPInteractorComponent()
	: IsHoldingItem(false)
{
	PrimaryComponentTick.bCanEverTick = true;
}


void URPInteractorComponent::BeginPlay()
{
	Super::BeginPlay();
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

void URPInteractorComponent::PickUpItem()
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	if (IsValid(PlayerCharacter))
	{
		// 청소 도구.
		ARPTestItemActor* TestItemActor = Cast<ARPTestItemActor>(PlayerCharacter->GetHitResult().GetActor());
		if (IsValid(TestItemActor))
		{
			Server_PickUpItem(TestItemActor);
		}
	}
}

bool URPInteractorComponent::Server_PickUpItem_Validate(ARPTestItemActor* TargetActor)
{
	return IsValid(TargetActor);
}


void URPInteractorComponent::Server_PickUpItem_Implementation(ARPTestItemActor* TargetActor)
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
		IRPClickInterface* ClickInterface = Cast<IRPClickInterface>(PlayerCharacter->GetHitResult().GetActor());
		if (ClickInterface)
		{
			IRPClickInterface::Execute_ClickInteract(PlayerCharacter->GetHitResult().GetActor(), GetOwner());
		}

		IRPDragInterface* DragInterface = Cast<IRPDragInterface>(PlayerCharacter->GetHitResult().GetActor());
		if (DragInterface)
		{
			IRPDragInterface::Execute_DragInteract(PlayerCharacter->GetHitResult().GetActor(), GetOwner());
			IsHoldingItem = true;
			HoldingActor = PlayerCharacter->GetHitResult().GetActor();
		}


		// KeyHold로 이전.
		
		//IRPKeyHoldInterface* KeyHoldInterface = Cast<IRPKeyHoldInterface>(PlayerCharacter->GetHitResult().GetActor());
		//if (ClickInterface)
		//{
		//	//IRPKeyHoldInterface::Execute_ClickInteract(PlayerCharacter->GetHitResult().GetActor(), GetOwner());
		//}

		//bool HasClickInterface = PlayerCharacter->GetHitResult().GetActor()->Implements<URPClickInterface>();
	}
}

bool URPInteractorComponent::Server_Interact_Validate()
{
	return true;
}


void URPInteractorComponent::Server_Interact_Implementation()
{
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

	if (InteractActor == PlayerCharacter->GetHitResult().GetActor())
		return;

	if (IsValid(InteractWidget))
	{
		if (!IsValid(PlayerCharacter->GetHitResult().GetActor()) || IsHoldingItem)
		{
			InteractActor = nullptr;
			InteractWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

		ARPTestItemActor* TestItemActor = Cast<ARPTestItemActor>(PlayerCharacter->GetHitResult().GetActor());
		if (TestItemActor)
		{
			InteractActor = TestItemActor;
			InteractWidget->SetVisibility(ESlateVisibility::Visible);
		}


		ARPBaseInteractableObject* InteractableObjcet = Cast<ARPBaseInteractableObject>(PlayerCharacter->GetHitResult().GetActor());
		if (InteractableObjcet)
		{
			InteractActor = InteractableObjcet;
			InteractWidget->SetVisibility(ESlateVisibility::Visible);			
		}
	}
}

void URPInteractorComponent::OnLeftMouseButtonReleased()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Mouse Left Button Released!"));

	IRPDragInterface* DragInterface = Cast<IRPDragInterface>(HoldingActor);
	if (DragInterface)
	{
		IRPDragInterface::Execute_DropInteract(HoldingActor, GetOwner());
		IsHoldingItem = false;
		HoldingActor = nullptr;
	}
}