// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPInteractorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESETPROTOCOL_API URPInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URPInteractorComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void CreateInteractWidget(AController* Controller);

public:
	UFUNCTION(BlueprintCallable, Category = "Interact")
	FVector GetInteractEnd() { return InteractEnd; }

public:
	UFUNCTION(BlueprintCallable, Category = "Interact")
	void InteractCheck();

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void PickUpItem();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PickUpItem(ARPTestItemActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void Interact();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Interact();

public:
	void OnLeftMouseButtonReleased();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Interact")
	FVector ViewVector;

	UPROPERTY(EditDefaultsOnly, Category = "Interact")
	FVector InteractEnd;

	UPROPERTY(VisibleAnywhere, Category = "Interact")
	FRotator ViewRotation;

	UPROPERTY(EditDefaultsOnly, Category = "Interact")
	float InteractionRange;

	UPROPERTY(EditDefaultsOnly, Category = "Interact")
	TSubclassOf<UUserWidget> InteractWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "Interact")
	UUserWidget* InteractWidget;

	UPROPERTY(VisibleAnywhere)
	AActor* InteractActor;

	UPROPERTY(VisibleAnywhere)
	AActor* HoldingActor;

	UPROPERTY(VisibleAnywhere)
	bool IsHoldingItem;
		
};
