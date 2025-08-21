// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPMovableComponent.generated.h"

class UBoxComponent;
class UPhysicsConstraintComponent;

UENUM()
enum class ERPRootMode : uint8
{
	Box,
	Mesh
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESETPROTOCOL_API URPMovableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URPMovableComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Pickup(AActor* Interactor);
	void Grab(AActor* Interactor);
	void Drop();

	bool GetIsHeld() { return bIsHeld; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_RootMode();

	void ApplyRootSwap(ERPRootMode Mode, UPrimitiveComponent* NewRoot, UPrimitiveComponent* Other);

	// Mesh를 할당한 MeshComponent 찾기
	UMeshComponent* FindOwnerMeshComponent(AActor* Owner) const;
	// 액터를 부착할 Interactor의 컴포넌트 찾기
	USceneComponent* FindAnchor(AActor* Interactor) const;
	bool IsRootPhysicsActive();	

	void OnPickupComplete(AActor* Interactor);
	void OnPlaceComplete();

public:
	UPROPERTY(ReplicatedUsing = OnRep_RootMode)
	ERPRootMode RootMode;

private:
	// Physics Handle 사용
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> Holder;
	UPROPERTY(Transient)
	TWeakObjectPtr<UPrimitiveComponent> CacheGrabbedComp;
	UPROPERTY(Transient)
	TWeakObjectPtr<UPrimitiveComponent> CacheRootBox;
	UPROPERTY(Transient)
	TWeakObjectPtr<UPrimitiveComponent> CacheOwnerMesh;

	UPROPERTY(Replicated)
	bool bIsHeld;

	FQuat DeltaAnchorTarget;
	bool bSwappingRoot;
	bool bIsPickup;
};
