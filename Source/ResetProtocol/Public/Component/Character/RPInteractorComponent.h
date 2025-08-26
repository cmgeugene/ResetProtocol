// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerManager.h"
#include "Data/RPInteractUIData.h"
#include "RPInteractorComponent.generated.h"

class URPInteractWidget;
class URPRadialTimerWidget;
class ARPBaseInteractableObject;

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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void CreateInteractWidget(AController* Controller);

public:
	UFUNCTION(BlueprintCallable, Category = "Interact")
	FVector GetInteractEnd() { return InteractEnd; }

	UFUNCTION(BlueprintCallable, Category = "Interact")
	URPRadialTimerWidget* GetRedialTimerWidget() { return RedialTimerWidget; };


public:
	UFUNCTION(BlueprintCallable, Category = "Interact")
	void InteractCheck();

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void PickUpItem();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PickUpItem(ARPBaseCleaningTool* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void Interact();
	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void KeyHoldInteract();
	UFUNCTION(BlueprintCallable, Category = "Interact")
	void KeyReleaseInteract();
	UFUNCTION(BlueprintCallable, Category = "Interact")
	void KeyHoldTimerEnd();
	UFUNCTION(Server, Reliable)
	void Server_KeyHoldRPC(AActor* Target);
	UFUNCTION(Server, Reliable)
	void Server_KeyReleaseRPC();

	UFUNCTION(Server, Reliable)
	void Server_MouseReleaseInteract();
public:
	void UpdateInteractWidget(ARPBaseInteractableObject* InteractableObjcet);

public:
	void OnLeftMouseButtonReleased();

protected:
	void SetOwnerInteractHitResult();

public:
	UFUNCTION(Client, Reliable)
	void ShowRedialTimerWidget(bool IsVisibility, float Time = 0.0f);

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
	URPInteractWidget* InteractWidget;

	UPROPERTY(VisibleAnywhere)
	AActor* InteractActor;
	UPROPERTY(Replicated, VisibleAnywhere)
	AActor* HoldingActor;
	UPROPERTY(Replicated, VisibleAnywhere)
	bool IsHoldingItem;

	UPROPERTY(VisibleAnywhere)
	bool IsKeyRelease;
	UPROPERTY(VisibleAnywhere)
	float KeyHoldingTime;

	FTimerHandle KeyHoldTimerHandle;
		
	UPROPERTY(EditDefaultsOnly, Category = "InteractUI")
	URPInteractUIData* InteractUIData;

	UPROPERTY(EditDefaultsOnly, Category = "Redial")
	TSubclassOf<UUserWidget> RedialTimerWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite ,Category = "Redial")
	URPRadialTimerWidget* RedialTimerWidget;

	UPROPERTY(Replicated, BlueprintReadWrite)
	bool IsHoldingDragObject;

};
