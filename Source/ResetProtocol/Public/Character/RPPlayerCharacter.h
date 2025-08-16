#pragma once

#include "CoreMinimal.h"
#include "Character/RPBaseCharacter.h"
#include "Component/Character/RPInteractorComponent.h"
#include "Component/Character/RPInventoryComponent.h"
#include "Component/Character/RPHotbarComponent.h"
#include "Data/ItemDataBase.h"
#include "Net/UnrealNetwork.h"
#include "RPPlayerCharacter.generated.h"

class UInventoryWidget;
class UPhysicsHandleComponent;

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

	URPHotbarComponent* GetHotbarComponent() { return HotbarComponent; }

	URPInteractorComponent* GetInteractorComponent() { return InteractorComponent; }

	URPInventoryComponent* GetInventoryComponent() { return InventoryComponent; }

public:
	void OnLeftMouseButtonReleased();


// Component
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	URPInteractorComponent* InteractorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	URPInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	URPHotbarComponent* HotbarComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	UPhysicsHandleComponent* GrabComponent;


protected:
	
	UPROPERTY(VisibleAnywhere, Category = "Interact")
	FHitResult InteractHitResult;

};
