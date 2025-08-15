// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPMovableComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESETPROTOCOL_API URPMovableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URPMovableComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// RPC
	UFUNCTION(Server, Reliable)
	void Server_Pickup();
	UFUNCTION(Server, Reliable)
	void Server_Grab(AActor* Interactor);
	UFUNCTION(Server, Reliable)
	void Server_Drop();

	UFUNCTION(NetMulticast, Reliable)
	void Mulitcast_ApplyHoldCollision();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RestoreCollision();

	void OnPlaceComplete();
	bool IsPhysicsActive();

	bool GetIsHeld() { return bIsHeld; }

protected:
	virtual void BeginPlay() override;

	// Mesh를 할당한 MeshComponent 찾기
	UMeshComponent* FindOwnerMeshComponent(AActor* Owner) const;
	// 액터를 부착할 Interactor의 컴포넌트 찾기
	USceneComponent* FindAnchor(AActor* Interactor) const;

private:
	UPROPERTY()
	TWeakObjectPtr<AActor> Holder;
	UPROPERTY()
	TWeakObjectPtr<UMeshComponent> GrabbedMesh;

	UPROPERTY(Replicated)
	bool bIsHeld;


};
