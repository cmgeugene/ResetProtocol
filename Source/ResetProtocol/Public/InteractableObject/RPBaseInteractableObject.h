// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RPBaseInteractableObject.generated.h"

#define CUSTOM_DEPTH_RED 250
class UBoxComponent;
class UGlitchNoiseComponent;

UENUM(BlueprintType)
enum class EInteractObjectType : uint8
{
	None				UMETA(DisplayName = "None"),
	SomethingElse       UMETA(DisplayName = "SomethingElse"),
	Trash				UMETA(DisplayName = "Trash"),
	ScatteredObject		UMETA(DisplayName = "ScatteredObject"),
	Corpse				UMETA(DisplayName = "Corpse"),
	Trap				UMETA(DisplayName = "Trap"),
	Stain				UMETA(DisplayName = "Stain")
};

UCLASS()
class RESETPROTOCOL_API ARPBaseInteractableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	ARPBaseInteractableObject();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void Highlight();
	void UnHighlight();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> RootBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGlitchNoiseComponent> GlitchNoiseComp;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Type")
	EInteractObjectType ObjectType;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Type|Name")
	FName ObjectName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Type")
	bool bIsBug = false;

	// 현재 사용하고 있는 Mesh
	TObjectPtr<UMeshComponent> ActiveMesh;
};
