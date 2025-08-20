// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObject/RPBaseInteractableObject.h"
#include "Interface/RPDragInterface.h"
#include "Interface/RPKeyHoldInterface.h"
#include "RPCorpse.generated.h"

class URPMovableComponent;
class URPRagdollComponent;

UCLASS()
class RESETPROTOCOL_API ARPCorpse : public ARPBaseInteractableObject, public IRPDragInterface, public IRPKeyHoldInterface
{
	GENERATED_BODY()

public:
	ARPCorpse();

	virtual void Tick(float DeltaTime) override;

	virtual void DragInteract_Implementation(AActor* Interactor) override;
	virtual void DropInteract_Implementation(AActor* Interactor) override;
	virtual void KeyHoldInteract_Implementation(AActor* Interactor) override;
	virtual void KeyReleaseInteract_Implementation(AActor* Interactor) override;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URPMovableComponent> MoveComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URPRagdollComponent> RagdollComp;
};
