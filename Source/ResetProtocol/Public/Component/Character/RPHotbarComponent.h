#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ItemDataBase.h"
#include "Data/RPCleaningToolData.h"
#include "RPHotbarComponent.generated.h"

class URPHotbarWidget;
class ARPBaseCleaningTool;

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
	URPCleaningToolData* GetItemDataBase() { return ItemDataBase; }
	TArray<FCleaningToolData>& GetInventory() { return Inventory; }

	UFUNCTION(BlueprintCallable)
	void SetHotbarWidget(URPHotbarWidget* Widget) { HotbarWidget = Widget; }

	ARPBaseCleaningTool* GetCurrentCleaningTool() { return CurrentCleaningTool; }

// Widget
public:
	UFUNCTION(Client, Reliable)
	void CreateHotbarWidget(AController* Controller);

// Inventory Function
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DropItem(const FCleaningToolData& DroppedItem);

	UFUNCTION()
	void OnRep_Inventory();

	UFUNCTION(Client, Reliable)
	void AddItem(const FCleaningToolData& Data);

	UFUNCTION()
	bool CheckInventoryFull();

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
	URPCleaningToolData* ItemDataBase;

	UPROPERTY(ReplicatedUsing = OnRep_Inventory, VisibleAnywhere, Category = "Inventory")
	TArray<FCleaningToolData> Inventory;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UUserWidget> HotbarWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	URPHotbarWidget* HotbarWidget;

	UPROPERTY(VisibleAnywhere)
	int CurrentSlotIndex;

	UPROPERTY(VisibleAnywhere)
	ARPBaseCleaningTool* CurrentCleaningTool;
};
