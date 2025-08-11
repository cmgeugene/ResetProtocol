// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObject/RPBaseInteractableObject.h"
#include "Interface/RPClickInterface.h"
#include "RPStain.generated.h"

class URPCleanableComponent;

UCLASS()
class RESETPROTOCOL_API ARPStain : public ARPBaseInteractableObject, public IRPClickInterface
{
	GENERATED_BODY()
	
public:
	ARPStain();

	virtual void ClickInteract_Implementation(AActor* Interactor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URPCleanableComponent> CleanComp;
};
