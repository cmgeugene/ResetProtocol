#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ItemDataBase.h"
#include "RPHotbarComponent.generated.h"

class URPHotbarWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESETPROTOCOL_API URPHotbarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URPHotbarComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

// Get & Set
public:
	URPHotbarWidget* GetInventoryWidget() { return HotbarWidget; }
	UItemDataBase* GetItemDataBase() { return ItemDataBase; }
	TArray<FItemData>& GetInventory() { return Inventory; }

// Widget
public:
	UFUNCTION(Client, Reliable)
	void CreateHotbarWidget(AController* Controller);

// Inventory Function
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DropItem(const FItemData& DroppedItem);

	UFUNCTION()
	void OnRep_Inventory();

	UFUNCTION(Client, Reliable)
	void AddItem(const FItemData& Data);

// Item
public:

	UFUNCTION(BlueprintCallable)
	void SelectItem(int SelectedNum);

	UFUNCTION(BlueprintCallable)
	void UnEquip();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int TotalItems;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	UItemDataBase* ItemDataBase;

	UPROPERTY(ReplicatedUsing = OnRep_Inventory, VisibleAnywhere, Category = "Inventory")
	TArray<FItemData> Inventory;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UUserWidget> HotbarWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	URPHotbarWidget* HotbarWidget;

	UPROPERTY(VisibleAnywhere)
	int CurrentSlotIndex;
};
