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

	UFUNCTION(BlueprintCallable)
	URPCleaningToolData* GetItemDataBase() { return ItemDataBase; }
	TArray<FCleaningToolData>& GetInventory() { return Inventory; }

	UFUNCTION(BlueprintCallable)
	void SetHotbarWidget(URPHotbarWidget* Widget) { HotbarWidget = Widget; }

	UFUNCTION(BlueprintCallable)
	ARPBaseCleaningTool* GetCurrentCleaningTool() { return CurrentCleaningTool; }

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetCurrentIndex(int Index);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void UpdateUI();

// Widget
public:
	UFUNCTION(Client, Reliable)
	void Client_CreateHotbarWidget(AController* Controller);

// Inventory Function
public:
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_DropItem();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SpawnCleaningTool(TSubclassOf<ARPBaseCleaningTool> ActorClass, FVector SpawnLocation);

	UFUNCTION()
	void OnRep_Inventory();
	UFUNCTION()
	void OnRep_CurrentCleaningTool();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_AddItem(const FCleaningToolData& Data);

	UFUNCTION()
	bool CheckInventoryFull();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void FillInventory();

// Item
public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SpawnActor(TSubclassOf<ARPBaseCleaningTool> ActorClass);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_AttachTool(ARPBaseCleaningTool* SpawnedTool);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_DestroyActor();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_SelectItem(int SelectedNum);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_UnEquip();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int TotalItems;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	URPCleaningToolData* ItemDataBase;

	UPROPERTY(ReplicatedUsing = OnRep_Inventory, VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FCleaningToolData> Inventory;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UUserWidget> HotbarWidgetClass;
	UPROPERTY(VisibleAnywhere, Replicated, Category = "Inventory")
	URPHotbarWidget* HotbarWidget;

	UPROPERTY(VisibleAnywhere, Replicated)
	int CurrentSlotIndex;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_CurrentCleaningTool)
	//UPROPERTY(Replicated)
	ARPBaseCleaningTool* CurrentCleaningTool;
};
