#pragma once

#include "CoreMinimal.h"
#include "Character/RPBaseCharacter.h"
#include "Component/RPInteractorComponent.h"
#include "Data/ItemDataBase.h"
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

public:
	FHitResult& GetHitResult() { return InteractHitResult; }

	FVector GetInteractEnd() { return InteractorComponent->GetInteractEnd(); }

	UItemDataBase* GetItemDataBase() { return ItemDataBase; }

	TArray<FItemData>& GetInventory() { return Inventory; }
	
	UInventoryWidget* GetInventoryWidget() { return InventoryWidget; }

protected:
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ToggleInventory();

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	URPInteractorComponent* InteractorComponent;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	UInventoryWidget* InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	UItemDataBase* ItemDataBase;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<FItemData> Inventory;

protected:
	
	UPROPERTY(VisibleAnywhere, Category = "Interact")
	FHitResult InteractHitResult;

};
