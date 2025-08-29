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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void KeyHoldInteract_Implementation(AActor* Interactor) override;
	virtual void KeyReleaseInteract_Implementation(AActor* Interactor) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AfterRepairProcess();
	void AfterRepairProcess_Implementation();

	UFUNCTION()
	void OnRep_IsBroken();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void AfterBeginPlayProcess();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BrokenMesh;

	UPROPERTY(ReplicatedUsing = "OnRep_IsBroken", VisibleAnywhere, BlueprintReadWrite, Category = "State")
	bool bIsBroken;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URPRepairableComponent> RepairComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	int32 BrokenProbability;
};
