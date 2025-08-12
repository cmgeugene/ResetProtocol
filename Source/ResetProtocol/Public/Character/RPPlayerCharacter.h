#pragma once

#include "CoreMinimal.h"
#include "Character/RPBaseCharacter.h"
#include "Component/RPInteractorComponent.h"
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

	UItemDataBase* GetItemDataBase() { return ItemDataBase; }

	TArray<FItemData>& GetInventory() { return Inventory; }
	
	UInventoryWidget* GetInventoryWidget() { return InventoryWidget; }

// Input
protected:
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ToggleInventory();

// Inventory Function
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DropItem(const FItemData& DroppedItem, FVector SpawnLocation);

	UFUNCTION()
	void OnRep_Inventory();

// Component
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	URPInteractorComponent* InteractorComponent;

// Inventory
protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	UInventoryWidget* InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	UItemDataBase* ItemDataBase;

	UPROPERTY(ReplicatedUsing = OnRep_Inventory, VisibleAnywhere, Category = "Inventory")
	TArray<FItemData> Inventory;

protected:
	
	UPROPERTY(VisibleAnywhere, Category = "Interact")
	FHitResult InteractHitResult;

};
