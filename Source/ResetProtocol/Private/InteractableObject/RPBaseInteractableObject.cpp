// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject/RPBaseInteractableObject.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

#define ECC_ObjectRootBox ECC_GameTraceChannel2
#define ECC_ObjectSkeletalMesh ECC_GameTraceChannel3
#define ECC_ObjectStaticMesh ECC_GameTraceChannel4

ARPBaseInteractableObject::ARPBaseInteractableObject()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	RootBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RootBox"));
	RootBox->SetSimulatePhysics(true);
	RootBox->SetCollisionProfileName(TEXT("ResetObjectRoot"));
	SetRootComponent(RootBox);

	// StaticMesh는 Physics Off => Root의 Collision을 무시할 필요 없음
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(RootComponent);
	StaticMeshComp->SetCollisionProfileName(TEXT("ResetObjectStatic"));
	StaticMeshComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	StaticMeshComp->SetIsReplicated(true);

	// SkeletalMesh는 Physics On/Off
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("USkeletalMeshComp"));
	//SkeletalMeshComp->SetSimulatePhysics(true);
	SkeletalMeshComp->SetupAttachment(RootComponent);
	SkeletalMeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	SkeletalMeshComp->SetCollisionProfileName(TEXT("ResetObjectSkeletal"));
	SkeletalMeshComp->SetIsReplicated(true);
}

void ARPBaseInteractableObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPBaseInteractableObject, ObjectType);
}

void ARPBaseInteractableObject::BeginPlay()
{
	Super::BeginPlay();
	
	if (StaticMeshComp && StaticMeshComp->GetStaticMesh())
	{
		SkeletalMeshComp->SetVisibility(false);
		ActiveMesh = StaticMeshComp;
	}
	else if (SkeletalMeshComp && SkeletalMeshComp->GetSkeletalMeshAsset())
	{
		StaticMeshComp->SetVisibility(false);
		ActiveMesh = SkeletalMeshComp;
	}
}

void ARPBaseInteractableObject::Highlight()
{
	// SetRenderCustomDepth()/SetCustomDepthStencilValue()는 PrimitiveComponent의 함수
	if (ActiveMesh)
	{
		if (UStaticMeshComponent* ActiveStaticMesh = Cast<UStaticMeshComponent>(ActiveMesh))
		{
			ActiveStaticMesh->SetRenderCustomDepth(true);
			ActiveStaticMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
		}
		else if (USkeletalMeshComponent* ActiveSkeletalMesh = Cast<USkeletalMeshComponent>(ActiveMesh))
		{
			ActiveStaticMesh->SetRenderCustomDepth(true);
			ActiveStaticMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
		}
	}
}

void ARPBaseInteractableObject::UnHighlight()
{
	if (ActiveMesh)
	{
		if (UStaticMeshComponent* ActiveStaticMesh = Cast<UStaticMeshComponent>(ActiveMesh))
		{
			ActiveStaticMesh->SetRenderCustomDepth(false);
		}
		else if (USkeletalMeshComponent* ActiveSkeletalMesh = Cast<USkeletalMeshComponent>(ActiveMesh))
		{
			ActiveStaticMesh->SetRenderCustomDepth(false);
		}
	}
}