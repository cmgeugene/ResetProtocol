// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObject/RPBaseInteractableObject.h"
#include "Interface/RPKeyHoldInterface.h"
#include "RPTrap.generated.h"

class URPRepairableComponent;

UCLASS()
class RESETPROTOCOL_API ARPTrap : public ARPBaseInteractableObject, public IRPKeyHoldInterface
{
	GENERATED_BODY()
	
public:
	ARPTrap();

	virtual void KeyHoldInteract_Implementation(AActor* Interactor) override;
	virtual void KeyReleaseInteract_Implementation(AActor* Interactor) override;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BrokenMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URPRepairableComponent> RepairComp;
};
