#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RPBaseCleaningTool.generated.h"

UENUM(BlueprintType)
enum class ECleaningToolState : uint8
{
	Broom UMETA(DisplayName = "Broom"),

	Mop UMETA(DisplayName = "Mop"),

	Vacuum UMETA(DisplayName = "Vacuum"),

	Hammer UMETA(DisplayName = "Hammer"),

	Max
};

UENUM(BlueprintType)
enum class EPurchaseState : uint8
{
	Purchased UMETA(DisplayName = "Purchased"),

	NotPurchased UMETA(DisplayName = "NotPurchased"),
	
	Max
};

UCLASS()
class RESETPROTOCOL_API ARPBaseCleaningTool : public AActor
{
	GENERATED_BODY()
	
public:	
	ARPBaseCleaningTool();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	ECleaningToolState GetCleaningToolState() { return eCleaningToolState; }

	UStaticMeshComponent* GetMesh() { return StaticMeshComp; }

	EPurchaseState GetPurchaseState() { return ePurchaseState; }

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Variable")
	ECleaningToolState eCleaningToolState;

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Variable")
	EPurchaseState ePurchaseState;
};
