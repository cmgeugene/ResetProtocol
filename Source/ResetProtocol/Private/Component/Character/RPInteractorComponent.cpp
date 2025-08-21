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
			bool CanPickUp = PlayerCharacter->GetHotbarComponent()->CheckInventoryFull();
			if (CanPickUp)
			{
				if (TargetActor->GetPurchaseState() == EPurchaseState::NotPurchased)
				{
					// 구매 로직 
					int Price = Data->Price;

					if (PlayerCharacter->SpendBytes(Price))
					{
						PlayerCharacter->GetHotbarComponent()->AddItem(*Data);

						TargetActor->Destroy();
					}
				}
				else
				{
					PlayerCharacter->GetHotbarComponent()->AddItem(*Data);

					TargetActor->Destroy();
				}
			}
		}
	}
}

void URPInteractorComponent::Interact()
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	if (IsValid(PlayerCharacter) && HoldingActor == nullptr)
	{
		ARPBaseInteractableObject* InteractableObject = Cast<ARPBaseInteractableObject>(PlayerCharacter->GetHitResult().GetActor());
			
		ARPBaseCleaningTool* CleaningTool = PlayerCharacter->GetHotbarComponent()->GetCurrentCleaningTool();

		IRPClickInterface* ClickInterface = Cast<IRPClickInterface>(InteractableObject);
		if (ClickInterface)
		{
			if (InteractableObject->ObjectType == EInteractObjectType::Trash)
			{
				if (!IsValid(CleaningTool))
				{
					IRPClickInterface::Execute_ClickInteract(InteractableObject, GetOwner());
				}
			}
			else if (InteractableObject->ObjectType == EInteractObjectType::Stain)
			{
				if (IsValid(CleaningTool) && CleaningTool->GetCleaningToolState() == ECleaningToolState::Mop)
				{
					IRPClickInterface::Execute_ClickInteract(InteractableObject, GetOwner());
				}
			}
		}

		IRPDragInterface* DragInterface = Cast<IRPDragInterface>(InteractableObject);
		if (DragInterface)
		{
			if (InteractableObject->ObjectType == EInteractObjectType::ScatteredObject || InteractableObject->ObjectType == EInteractObjectType::Corpse)
			{
				if (!IsValid(CleaningTool))
				{
					IRPDragInterface::Execute_DragInteract(InteractableObject, GetOwner());
					IsHoldingItem = true;
					HoldingActor = InteractableObject;
				}
			}
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
			InteractWidget->RefreshWidget();
			InteractWidget->InvisiblePrice();
			InteractWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

		ARPBaseCleaningTool* CleaningTool = Cast<ARPBaseCleaningTool>(PlayerCharacter->GetHitResult().GetActor());
		if (CleaningTool)
		{
			InteractWidget->SetText(TEXT("PickUp CleaningTool"));

			if (CleaningTool->GetPurchaseState() == EPurchaseState::NotPurchased)
			{
				FCleaningToolData* Data = PlayerCharacter->GetHotbarComponent()->GetItemDataBase()->Items.FindByPredicate([&](const FCleaningToolData& ItemData)
					{
						return ItemData.Class == CleaningTool->GetClass();
					});

				InteractWidget->VisiblePrice(Data->Price);
			}

			FInteractUIData Data = InteractUIData->FindItem("PickUp");

			TArray<FInteractUIData> Datas;
			Datas.Add(Data);

			InteractWidget->AddList(Datas);


			InteractActor = CleaningTool;
			InteractWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
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

			InteractWidget->InvisiblePrice();

			UpdateInteractWidget(InteractableObjcet);

			InteractActor = InteractableObjcet;
			InteractWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}

void URPInteractorComponent::KeyHoldInteract()
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	if (IsValid(PlayerCharacter) && HoldingActor == nullptr)
	{
		ARPBaseInteractableObject* InteractableObjcet = Cast<ARPBaseInteractableObject>(PlayerCharacter->GetHitResult().GetActor());

		ARPBaseCleaningTool* CleaningTool = PlayerCharacter->GetHotbarComponent()->GetCurrentCleaningTool();
		
		IRPKeyHoldInterface* KeyHoldInterface = Cast<IRPKeyHoldInterface>(InteractableObjcet);
		if (KeyHoldInterface)
		{
			if (InteractableObjcet->ObjectType == EInteractObjectType::Trap || InteractableObjcet->ObjectType == EInteractObjectType::Corpse)
			{
				if (IsValid(CleaningTool) && CleaningTool->GetCleaningToolState() == ECleaningToolState::Hammer)
				{
					IsHoldingItem = true;
					HoldingActor = PlayerCharacter->GetHitResult().GetActor();

					GetWorld()->GetTimerManager().SetTimer(
						KeyHoldTimerHandle,
						this,
						&URPInteractorComponent::KeyHoldTimerEnd,
						KeyHoldingTime,
						false
					);
				}
			}
		}
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

void URPInteractorComponent::UpdateInteractWidget(ARPBaseInteractableObject* InteractableObjcet)
{
	TArray<FInteractUIData> Datas;
	FInteractUIData Data = InteractUIData->FindItem("PickUp");

	EInteractObjectType Type = InteractableObjcet->ObjectType;

	if (Type == EInteractObjectType::Trash)
	{
		Data = InteractUIData->FindItem("Handle");
		Datas.Add(Data);
	}
	else if (Type == EInteractObjectType::Stain)
	{
		Data = InteractUIData->FindItem("PickUp");
		Datas.Add(Data);

		Data = InteractUIData->FindItem("Clean");
		Datas.Add(Data);
	}
	else if (Type == EInteractObjectType::ScatteredObject)
	{
		Data = InteractUIData->FindItem("Handle");
		Datas.Add(Data);
	}
	else if (Type == EInteractObjectType::Trap)
	{
		Data = InteractUIData->FindItem("Repair");
		Datas.Add(Data);
	}
	else if (Type == EInteractObjectType::Corpse)
	{
		Data = InteractUIData->FindItem("Handle");
		Datas.Add(Data);

		Data = InteractUIData->FindItem("Repair");
		Datas.Add(Data);
	}


	InteractWidget->AddList(Datas);
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