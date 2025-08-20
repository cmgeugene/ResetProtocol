// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObject/RPBaseInteractableObject.h"
#include "Interface/RPDragInterface.h"
#include "RPScatteredObject.generated.h"

class URPMovableComponent;

UCLASS()
class RESETPROTOCOL_API ARPScatteredObject : public ARPBaseInteractableObject, public IRPDragInterface
{
	GENERATED_BODY()

public:
	ARPScatteredObject();

	virtual void DragInteract_Implementation(AActor* Interactor) override;
	virtual void DropInteract_Implementation(AActor* Interactor) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URPMovableComponent> MoveComp;
};
