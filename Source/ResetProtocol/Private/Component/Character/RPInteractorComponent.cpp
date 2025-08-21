#include "Component/Character/RPInteractorComponent.h"
#include "Character/RPPlayerCharacter.h"
#include "Frameworks/RPPlayerController.h"
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
#include "UI/RPRadialTimerWidget.h"

#include "Interface/RPClickInterface.h"
#include "Interface/RPDragInterface.h"
#include "Interface/RPKeyHoldInterface.h"

URPInteractorComponent::URPInteractorComponent()
	: IsHoldingItem(false)
	, IsKeyRelease(false)
	, KeyHoldingTime(5.f)
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}


void URPInteractorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URPInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (!IsValid(Pawn))
	{
		return;
	}

	if (Pawn->IsLocallyControlled())
	{
		InteractCheck();
	}
}

void URPInteractorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPInteractorComponent, HoldingActor);
	DOREPLIFETIME(URPInteractorComponent, IsHoldingItem);
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

	if (IsValid(RedialTimerWidgetClass))
	{
		RedialTimerWidget = Cast<URPRadialTimerWidget>(CreateWidget(Cast<APlayerController>(Controller), RedialTimerWidgetClass));

		if (IsValid(RedialTimerWidget))
		{
			RedialTimerWidget->AddToViewport(0);
			RedialTimerWidget->SetVisibility(ESlateVisibility::Collapsed);
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
		FCleaningToolData* Data = PlayerCharacter->GetHotbarComponent()->GetItemDataBase()->Items.FindByPredicate(
			[&](const FCleaningToolData& ItemData)
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
						PlayerCharacter->GetHotbarComponent()->Client_AddItem(*Data);

						TargetActor->Destroy();
					}
				}
				else
				{
					PlayerCharacter->GetHotbarComponent()->Client_AddItem(*Data);

					TargetActor->Destroy();
				}
			}
		}
	}
}

void URPInteractorComponent::Interact()
{
	if (ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner()))
	{
		AActor* Target = PlayerCharacter->GetHitResult().GetActor();
		if (Target)
		{
			Server_Interact(Target);
		}
	}
}

void URPInteractorComponent::Server_Interact_Implementation(AActor* Target)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Mouse Left Button Down!"));
	if (!IsValid(Target))
	{
		return;
	}
	
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());
	const float Distance = FVector::Dist(PlayerCharacter->GetActorLocation(), Target->GetActorLocation());
	if (Distance > InteractionRange)
	{
		return;
	}

	if (IsValid(PlayerCharacter) && HoldingActor == nullptr)
	{
		ARPBaseInteractableObject* InteractableObject = Cast<ARPBaseInteractableObject>(Target);
		ARPBaseCleaningTool* CleaningTool = PlayerCharacter->GetHotbarComponent()->GetCurrentCleaningTool();

		IRPClickInterface* ClickInterface = Cast<IRPClickInterface>(InteractableObject);
		if (ClickInterface)
		{
			if (InteractableObject->ObjectType == EInteractObjectType::Trash)
			{
				if (!IsValid(CleaningTool))
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Pick up Trash"));
					IRPClickInterface::Execute_ClickInteract(InteractableObject, GetOwner());
				}
			}
			else if (InteractableObject->ObjectType == EInteractObjectType::Stain)
			{
				if (IsValid(CleaningTool) && CleaningTool->GetCleaningToolState() == ECleaningToolState::Mop)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Clean Stain"));
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
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Grab Object"));
					IRPDragInterface::Execute_DragInteract(InteractableObject, GetOwner());
					IsHoldingItem = true;
					HoldingActor = InteractableObject;
				}
			}
		}
	}
}

void URPInteractorComponent::InteractCheck()
{
	SetOwnerInteractHitResult();

	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());
	if (!IsValid(PlayerCharacter))
	{
		return;
	}

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
	if (ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner()))
	{
		AActor* Target = PlayerCharacter->GetHitResult().GetActor();
		if (Target)
		{
			Server_KeyHoldRPC(Target);
		}
	}
}

void URPInteractorComponent::KeyReleaseInteract()
{
	if(ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner()))
	{
		if (HoldingActor) {
			Server_KeyReleaseRPC();
		}
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

void URPInteractorComponent::Server_KeyHoldRPC_Implementation(AActor* Target)
{
	if (!IsValid(Target))
	{
		return;
	}

	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());
	const float Distance = FVector::Dist(PlayerCharacter->GetActorLocation(), Target->GetActorLocation());
	if (Distance > InteractionRange)
	{
		return;
	}

	if (IsValid(PlayerCharacter) && HoldingActor == nullptr)
	{
		ARPBaseInteractableObject* InteractableObjcet = Cast<ARPBaseInteractableObject>(Target);
		ARPBaseCleaningTool* CleaningTool = PlayerCharacter->GetHotbarComponent()->GetCurrentCleaningTool();

		IRPKeyHoldInterface* KeyHoldInterface = Cast<IRPKeyHoldInterface>(InteractableObjcet);
		if (KeyHoldInterface)
		{
			if (InteractableObjcet->ObjectType == EInteractObjectType::Trap || InteractableObjcet->ObjectType == EInteractObjectType::Corpse)
			{
				if (IsValid(CleaningTool) && CleaningTool->GetCleaningToolState() == ECleaningToolState::Hammer)
				{
					IsHoldingItem = true;
					HoldingActor = InteractableObjcet;

					GetWorld()->GetTimerManager().SetTimer(
						KeyHoldTimerHandle,
						this,
						&URPInteractorComponent::KeyHoldTimerEnd,
						KeyHoldingTime,
						false
					);

					SetRedialTimerWidget(true);
				}
			}
		}
	}
}

void URPInteractorComponent::Server_KeyReleaseRPC_Implementation()
{
	if (KeyHoldTimerHandle.IsValid())
	{
		KeyHoldTimerHandle.Invalidate();
		IsHoldingItem = false;
		HoldingActor = nullptr;

		SetRedialTimerWidget(false);
	}
}

void URPInteractorComponent::Server_MouseReleaseInteract_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Mouse Left Button Released!"));

	IRPDragInterface* DragInterface = Cast<IRPDragInterface>(HoldingActor);
	if (DragInterface)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Execute_DropInteract"));
		IRPDragInterface::Execute_DropInteract(HoldingActor, GetOwner());
	}

	IsHoldingItem = false;
	HoldingActor = nullptr;

	SetRedialTimerWidget(false);

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
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());
	if (!PlayerCharacter)
	{
		return;
	}

	if (HoldingActor) {
		Server_MouseReleaseInteract();
	}

	ARPPlayerController* PlayerController = Cast<ARPPlayerController>(GetOwner()->GetInstigatorController());
	if (PlayerController)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->bShowMouseCursor = false;
	}
}

void URPInteractorComponent::SetOwnerInteractHitResult()
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
}

void URPInteractorComponent::SetRedialTimerWidget_Implementation(bool IsVisibility)
{
	if (IsVisibility)
	{
		RedialTimerWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		RedialTimerWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
