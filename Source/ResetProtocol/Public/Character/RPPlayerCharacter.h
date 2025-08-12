#pragma once

#include "CoreMinimal.h"
#include "Character/RPBaseCharacter.h"
#include "Component/Character/RPInteractorComponent.h"
#include "Component/Character/RPInventoryComponent.h"
#include "Data/ItemDataBase.h"
#include "Net/UnrealNetwork.h"
#include "RPPlayerCharacter.generated.h"

class UInventoryWidget;

UCLASS()
class RESETPROTOCOL_API ARPPlayerCharacter : public ARPBaseCharacter
{
	GENERATED_BODY()

public:
	ARPPlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

// Get & Set
public:
	FHitResult& GetHitResult() { return InteractHitResult; }

	FVector GetInteractEnd() { return InteractorComponent->GetInteractEnd(); }

	UItemDataBase* GetItemDataBase() { return InventoryComponent->GetItemDataBase(); }

	TArray<FItemData>& GetInventory() { return InventoryComponent->GetInventory(); }
	
	UInventoryWidget* GetInventoryWidget() { return InventoryComponent->GetInventoryWidget(); }

//// Input
//protected:
//	UFUNCTION(BlueprintCallable, Category = "Input")
//	void ToggleInventory();
//
//// Inventory Function
public:
	UFUNCTION()
	void Server_DropItem(const FItemData& DroppedItem) { InventoryComponent->Server_DropItem(DroppedItem); }
//
//	UFUNCTION()
//	void OnRep_Inventory();

// Component
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	URPInteractorComponent* InteractorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	URPInventoryComponent* InventoryComponent;

// Inventory
//protected:
//	
//	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
//	TSubclassOf<UUserWidget> InventoryWidgetClass;
//
//	UPROPERTY(VisibleAnywhere, Category = "Inventory")
//	UInventoryWidget* InventoryWidget;
//
//	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
//	UItemDataBase* ItemDataBase;
//
//	UPROPERTY(ReplicatedUsing = OnRep_Inventory, VisibleAnywhere, Category = "Inventory")
//	TArray<FItemData> Inventory;

protected:
	
	UPROPERTY(VisibleAnywhere, Category = "Interact")
	FHitResult InteractHitResult;

};
