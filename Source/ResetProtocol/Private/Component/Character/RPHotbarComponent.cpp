#include "Component/Character/RPHotbarComponent.h"
#include "Net/UnrealNetwork.h"

#include "Character/RPPlayerCharacter.h"
#include "UI/Inventory/RPHotbarWidget.h"
#include "RPTestItemActor.h"
#include "CleaningTool/RPBaseCleaningTool.h"

URPHotbarComponent::URPHotbarComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;

	TotalItems = 4;
	CurrentSlotIndex = -1;
}

void URPHotbarComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());
	//
	//if (PlayerCharacter->GetController()->IsLocalPlayerController())
	//{
	//	for (int i = 0; i < TotalItems; i++)
	//	{
	//		Inventory.Add(FCleaningToolData());
	//	}
	//}

	if (GetOwner()->HasAuthority() )
	{
		for (int i = 0; i < TotalItems; i++)
		{
			Inventory.Add(FCleaningToolData());
		}
	}
}

void URPHotbarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void URPHotbarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPHotbarComponent, Inventory);
	DOREPLIFETIME(URPHotbarComponent, CurrentSlotIndex);
	DOREPLIFETIME(URPHotbarComponent, CurrentCleaningTool);
	DOREPLIFETIME(URPHotbarComponent, HotbarWidget);
}

void URPHotbarComponent::Client_CreateHotbarWidget_Implementation(AController* Controller)
{
	//if (Controller->IsLocalPlayerController())
	//{
	//	for (int i = 0; i < TotalItems; i++)
	//	{
	//		Inventory.Add(FCleaningToolData());
	//	}
	//}

	if (IsValid(HotbarWidgetClass) && Controller->IsLocalPlayerController())
	{
		HotbarWidget = CreateWidget<URPHotbarWidget>(Cast<APlayerController>(Controller), HotbarWidgetClass);

		if (IsValid(HotbarWidget))
		{
			HotbarWidget->AddToViewport(0);
			HotbarWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}

bool URPHotbarComponent::Server_DropItem_Validate(const FCleaningToolData& DroppedItem)
{
	return DroppedItem.Class != nullptr;
}

void URPHotbarComponent::Server_DropItem_Implementation(const FCleaningToolData& DroppedItem)
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	if (IsValid(DroppedItem.Class) && IsValid(PlayerCharacter))
	{
		GetWorld()->SpawnActor<ARPTestItemActor>(DroppedItem.Class, PlayerCharacter->GetInteractorComponent()->GetInteractEnd(), FRotator::ZeroRotator);
	}
}

void URPHotbarComponent::OnRep_Inventory()
{
	if (IsValid(HotbarWidget))
	{
		HotbarWidget->UpdateUI();
	}
}

void URPHotbarComponent::OnRep_CurrentCleaningTool()
{
	if (CurrentCleaningTool && GetOwner())
	{
		ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

		USkeletalMeshComponent* ChildMesh = Cast<USkeletalMeshComponent>(
			PlayerCharacter->GetDefaultSubobjectByName(TEXT("RTG_Character"))
		);

		if (IsValid(ChildMesh))
		{
			if (ChildMesh->DoesSocketExist(FName("CleaningToolSocket")))
			{
				CurrentCleaningTool->GetMesh()->SetSimulatePhysics(false);
				CurrentCleaningTool->GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
				CurrentCleaningTool->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				FVector Scale = CurrentCleaningTool->GetClass()->GetDefaultObject<ARPBaseCleaningTool>()->GetMesh()->GetRelativeScale3D();
				CurrentCleaningTool->GetMesh()->SetRelativeScale3D(Scale);
				
				FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true); // Snap + KeepRelative
				CurrentCleaningTool->GetMesh()->AttachToComponent(ChildMesh, AttachRules, FName("CleaningToolSocket"));

				FTransform Transform = CurrentCleaningTool->GetClass()->GetDefaultObject<ARPBaseCleaningTool>()->GetMesh()->GetRelativeTransform();

				CurrentCleaningTool->GetMesh()->SetRelativeTransform(Transform);
			}
		}
	}
}

void URPHotbarComponent::Server_SpawnActor_Implementation(TSubclassOf<ARPBaseCleaningTool> ActorClass)
{
	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = Cast<APawn>(GetOwner());
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.bDeferConstruction = false; 

	CurrentCleaningTool = GetWorld()->SpawnActor<ARPBaseCleaningTool>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);

	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	USkeletalMeshComponent* ChildMesh = Cast<USkeletalMeshComponent>(
		PlayerCharacter->GetDefaultSubobjectByName(TEXT("RTG_Character"))
	);

	if (IsValid(ChildMesh))
	{
		if (ChildMesh->DoesSocketExist(FName("CleaningToolSocket")))
		{
			CurrentCleaningTool->GetMesh()->SetSimulatePhysics(false);
			CurrentCleaningTool->GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
			CurrentCleaningTool->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			FVector Scale = CurrentCleaningTool->GetClass()->GetDefaultObject<ARPBaseCleaningTool>()->GetMesh()->GetRelativeScale3D();
			CurrentCleaningTool->GetMesh()->SetRelativeScale3D(Scale);

			FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true); // Snap + KeepRelative
			CurrentCleaningTool->GetMesh()->AttachToComponent(ChildMesh, AttachRules, FName("CleaningToolSocket"));

			FTransform Transform = CurrentCleaningTool->GetClass()->GetDefaultObject<ARPBaseCleaningTool>()->GetMesh()->GetRelativeTransform();

			CurrentCleaningTool->GetMesh()->SetRelativeTransform(Transform);
		}
	}
}

void URPHotbarComponent::Multicast_AttachTool_Implementation(ARPBaseCleaningTool* SpawnedTool)
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	USkeletalMeshComponent* ChildMesh = Cast<USkeletalMeshComponent>(
		PlayerCharacter->GetDefaultSubobjectByName(TEXT("RTG_Character"))
	);

	if (IsValid(ChildMesh))
	{
		if (ChildMesh->DoesSocketExist(FName("CleaningToolSocket")))
		{
			SpawnedTool->GetMesh()->SetSimulatePhysics(false);
			SpawnedTool->GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
			SpawnedTool->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			FAttachmentTransformRules AttachRules(EAttachmentRule::KeepRelative, true); // Snap + KeepRelative
			SpawnedTool->GetMesh()->AttachToComponent(ChildMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("CleaningToolSocket"));

			FVector Scale = SpawnedTool->GetClass()->GetDefaultObject<ARPBaseCleaningTool>()->GetMesh()->GetRelativeScale3D();
			FRotator Rotator = SpawnedTool->GetClass()->GetDefaultObject<ARPBaseCleaningTool>()->GetMesh()->GetRelativeRotation();

			SpawnedTool->GetMesh()->SetRelativeScale3D(Scale);
			SpawnedTool->GetMesh()->SetRelativeRotation(Rotator);
		}
	}

}


void URPHotbarComponent::Server_DestroyActor_Implementation()
{
	CurrentCleaningTool->Destroy();
}

void URPHotbarComponent::Client_SelectItem_Implementation(int SelectedNum)
{
	HotbarWidget->OnHighlight(SelectedNum, CurrentSlotIndex);

	if (IsValid(CurrentCleaningTool))
	{
		Server_DestroyActor();
		//CurrentCleaningTool->Destroy();
	}

	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());

	if (IsValid(Inventory[SelectedNum].Class) && IsValid(PlayerCharacter))
	{
		Server_SpawnActor(Inventory[SelectedNum].Class);
		//CurrentCleaningTool = GetWorld()->SpawnActor<ARPBaseCleaningTool>(Inventory[SelectedNum].Class, FVector::ZeroVector, FRotator::ZeroRotator);
	}

	CurrentSlotIndex = SelectedNum;
}

void URPHotbarComponent::Client_UnEquip_Implementation()
{
	HotbarWidget->OffHighlight(CurrentSlotIndex);

	if (IsValid(CurrentCleaningTool))
	{
		CurrentCleaningTool->Destroy();
	}

	CurrentSlotIndex = -1;
}

void URPHotbarComponent::Client_AddItem_Implementation(const FCleaningToolData& Data)
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(GetOwner());
	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	for (int i = 0; i < Inventory.Num(); i++)
	{
		if (!IsValid(Inventory[i].Class))
		{
			Inventory[i] = Data;
			if (PlayerCharacter->IsLocallyControlled())
			{
				HotbarWidget->UpdateUI();
			}

			if (CurrentSlotIndex == -1)
			{
				return;
			}
			else
			{
				if (i == CurrentSlotIndex)
				{
					Client_SelectItem(i);
				}
				else
				{
					HotbarWidget->OnHighlight(CurrentSlotIndex, CurrentSlotIndex);
				}
				return;
			}
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Inventory Full"));
}

bool URPHotbarComponent::CheckInventoryFull()
{
	for (int i = 0; i < Inventory.Num();i++)
	{
		if (!IsValid(Inventory[i].Class))
		{
			return true;
		}
	}

	return false;
}

