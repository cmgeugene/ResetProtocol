
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ItemDataBase.h"
#include "RPInventoryComponent.generated.h"

class UInventoryWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESETPROTOCOL_API URPInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URPInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

// Input
protected:
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ToggleInventory(AController* Controller);

// Get & Set
public:
	UInventoryWidget* GetInventoryWidget() { return InventoryWidget; }
	UItemDataBase* GetItemDataBase() { return ItemDataBase; }
	TArray<FItemData>& GetInventory() { return Inventory; }


// Widget
public:
	void CreateInventoryWidget(AController* Controller);


// Inventory Function
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DropItem(const FItemData& DroppedItem);

	UFUNCTION()
	void OnRep_Inventory();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	UInventoryWidget* InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	UItemDataBase* ItemDataBase;

	UPROPERTY(ReplicatedUsing = OnRep_Inventory, VisibleAnywhere, Category = "Inventory")
	TArray<FItemData> Inventory;
		
};
