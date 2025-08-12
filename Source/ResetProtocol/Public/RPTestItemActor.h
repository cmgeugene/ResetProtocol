// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "RPTestItemActor.generated.h"

UENUM(BlueprintType)
enum class EMaterialState : uint8
{
    Cube UMETA(DisplayName = "Cube"),

    Sphere UMETA(DisplayName = "Sphere"),

    Con UMETA(DisplayName = "Con"),

    Max
};

UCLASS()
class RESETPROTOCOL_API ARPTestItemActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
    ARPTestItemActor();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    EMaterialState GetMaterialType() { return eMaterialType; }

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Variable")
    EMaterialState eMaterialType;

	
};
