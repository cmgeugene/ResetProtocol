// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObject/RPBaseInteractableObject.h"
#include "Interface/RPClickInterface.h"
#include "RPStain.generated.h"

class URPCleanableComponent;
class UDecalComponent;

UCLASS()
class RESETPROTOCOL_API ARPStain : public ARPBaseInteractableObject, public IRPClickInterface
{
	GENERATED_BODY()
	
public:
	ARPStain();

	virtual void ClickInteract_Implementation(AActor* Interactor) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleDirtChanged(float NewValue);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UDecalComponent> Decal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URPCleanableComponent> CleanComp;

	// 정적인 머티리얼/머티리얼 인스턴스의 Scalar 값들을 동적으로 변경할 수 있게 해줌
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DecalMID;

// 테스트용
protected:
	virtual void OnObjectOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
