// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPRagdollComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESETPROTOCOL_API URPRagdollComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URPRagdollComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void RagdollOn();
	void RagdollOff();

	// RPC
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RagdollOn();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RagdollOff();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_bInitRagdoll();

	void InitRagdoll(bool bOn);

public:
	UPROPERTY(ReplicatedUsing = OnRep_bInitRagdoll)
	bool bInitRagdoll;

	bool bIsRagdollOn;
	
};
