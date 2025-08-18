#include "Component/Character/RPInteractorComponent.h"
#include "Character/RPPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Data/ItemDataBase.h"
#include "RPTestItemActor.h"
#include "Data/RPCleaningToolData.h"
#include "CleaningTool/RPBaseCleaningTool.h"
#include "Component/Character/RPHotbarComponent.h"
#include "ResetProtocol/ResetProtocol.h"

#include "InteractableObject/RPBaseInteractableObject.h"
#include "UI/Interact/RPInteractWidget.h"

#include "Interface/RPClickInterface.h"
#include "Interface/RPDragInterface.h"
#include "Interface/RPKeyHoldInterface.h"

URPInteractorComponent::URPInteractorComponent()
	: IsHoldingItem(false)
	, IsKeyRelease(false)
	, KeyHoldingTime(5.f)
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
		InteractWidget = Cast<URPInteractWidget>(CreateWidget(Cast<APlayerController>(Controller), InteractWidgetClass));

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
		ARPBaseCleaningTool* CleaningTool = Cast<ARPBaseCleaningTool>(PlayerCharacter->GetHitResult().GetActor());
		if (IsValid(CleaningTool))
		{
			Server_PickUpItem(CleaningTool);
		}
	}
}

bool URPInteractorComponent::Server_PickUpItem_Validate(ARPBaseCleaningTool* TargetActor)
{
	return IsValid(TargetActor);
}


void URPInteractorComponent::Server_PickUpItem_Implementation(ARPBaseCleaningTool* TargetActor)
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());
	if (IsValid(PlayerCharacter) && IsValid(TargetActor))
	{
		FCleaningToolData* Data = PlayerCharacter->GetHotbarComponent()->GetItemDataBase()->Items.FindByPredicate([&](const FCleaningToolData& ItemData)
			{
				return ItemData.Class == TargetActor->GetClass();
			});

		if (Data != nullptr)
		{
			PlayerCharacter->GetHotbarComponent()->AddItem(*Data);

			bool CanPickUp = PlayerCharacter->GetHotbarComponent()->CheckInventoryFull();
		
			if (CanPickUp)
			{
				TargetActor->Destroy();
			}
		}
	}
}

void URPInteractorComponent::Interact()
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	if (IsValid(PlayerCharacter) && HoldingActor == nullptr)
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

		ARPBaseCleaningTool* CleaningTool = Cast<ARPBaseCleaningTool>(PlayerCharacter->GetHitResult().GetActor());
		if (CleaningTool)
		{
			InteractWidget->SetText(TEXT("PickUp CleaningTool"));

			InteractActor = CleaningTool;
			InteractWidget->SetVisibility(ESlateVisibility::Visible);
		}


		ARPBaseInteractableObject* InteractableObjcet = Cast<ARPBaseInteractableObject>(PlayerCharacter->GetHitResult().GetActor());
		if (InteractableObjcet)
		{
			EInteractObjectType Type = InteractableObjcet->ObjectType;

			UEnum* EnumPtr = StaticEnum<EInteractObjectType>();
			if (EnumPtr)
			{
				FString EnumString = EnumPtr->GetNameStringByValue((int64)Type);
				InteractWidget->SetText(EnumString);
			}

			InteractActor = InteractableObjcet;
			InteractWidget->SetVisibility(ESlateVisibility::Visible);			
		}
	}
}

void URPInteractorComponent::KeyHoldInteract()
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	if (IsValid(PlayerCharacter) && HoldingActor == nullptr)
	{
		IRPKeyHoldInterface* KeyHoldInterface = Cast<IRPKeyHoldInterface>(PlayerCharacter->GetHitResult().GetActor());
		if (KeyHoldInterface)
		{
			IsHoldingItem = true;
			HoldingActor = PlayerCharacter->GetHitResult().GetActor();
		}


		GetWorld()->GetTimerManager().SetTimer(
			KeyHoldTimerHandle,
			this,
			&URPInteractorComponent::KeyHoldTimerEnd,
			KeyHoldingTime, 
			false 
		);
	}
}

void URPInteractorComponent::KeyReleaseInteract()
{
	if (KeyHoldTimerHandle.IsValid())
	{
		KeyHoldTimerHandle.Invalidate();
		IsHoldingItem = false;
		HoldingActor = nullptr;
	}
}

void URPInteractorComponent::KeyHoldTimerEnd()
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	if (IsValid(PlayerCharacter) && KeyHoldTimerHandle.IsValid())
	{
		IRPKeyHoldInterface* KeyHoldInterface = Cast<IRPKeyHoldInterface>(HoldingActor);
		if (KeyHoldInterface)
		{
			IRPKeyHoldInterface::Execute_KeyHoldInteract(HoldingActor, GetOwner());
			IsHoldingItem = false;
			HoldingActor = nullptr;
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