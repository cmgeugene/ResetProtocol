// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObject/RPBaseInteractableObject.h"
#include "Interface/RPClickInterface.h"
#include "RPTrash.generated.h"

class URPMovableComponent;

UCLASS()
class RESETPROTOCOL_API ARPTrash : public ARPBaseInteractableObject, public IRPClickInterface
{
	GENERATED_BODY()
	
public:
	ARPTrash();

	virtual void ClickInteract_Implementation(AActor* Interactor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URPMovableComponent> MoveComp;

};
