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

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BrokenMesh;

	UPROPERTY(Replicated)
	bool bIsBroken;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URPRepairableComponent> RepairComp;

	
// 테스트용
protected:
	virtual void OnObjectOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
