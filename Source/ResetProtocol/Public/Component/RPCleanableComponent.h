// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPCleanableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDirtChanged, float, NewValue);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESETPROTOCOL_API URPCleanableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URPCleanableComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_StartCleaning();
	void StopCleaning();
	void Completecleaning();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_DirtAmount();

public:
	UPROPERTY(BlueprintAssignable, Category = "Clean|Delegate")
	FOnDirtChanged OnDirtChanged;

protected:
	UPROPERTY(ReplicatedUsing = "OnRep_DirtAmount")
	float DirtAmount;
};
