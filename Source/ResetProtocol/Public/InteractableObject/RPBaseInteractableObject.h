// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RPBaseInteractableObject.generated.h"

#define CUSTOM_DEPTH_RED 250
class UBoxComponent;

UCLASS()
class RESETPROTOCOL_API ARPBaseInteractableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	ARPBaseInteractableObject();
	
	void Highlight();
	void UnHighlight();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComp;

	// 현재 사용하고 있는 Mesh
	TObjectPtr<UMeshComponent> ActiveMesh;

// 테스트용
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> CollisionBox;

	UFUNCTION()
	virtual void OnObjectOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
