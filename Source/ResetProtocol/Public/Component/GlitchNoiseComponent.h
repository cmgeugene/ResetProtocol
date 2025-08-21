// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "GlitchNoiseComponent.generated.h"

class USphereComponent;
class ARPBaseInteractableObject;

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RESETPROTOCOL_API UGlitchNoiseComponent : public UAudioComponent
{
	GENERATED_BODY()
	
public:
	UGlitchNoiseComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glitch Noise Component")
	TObjectPtr<USphereComponent> SoundRange;

	UPROPERTY()
	TObjectPtr<ARPBaseInteractableObject> OwnerActor;

protected:
	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
