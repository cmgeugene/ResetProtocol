// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject/RPBaseInteractableObject.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

ARPBaseInteractableObject::ARPBaseInteractableObject()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(SceneRoot);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
	StaticMeshComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	StaticMeshComp->SetIsReplicated(true);

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("USkeletalMeshComp"));
	SkeletalMeshComp->SetupAttachment(SceneRoot);
	SkeletalMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkeletalMeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
	SkeletalMeshComp->SetIsReplicated(true);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollsionBox"));
	CollisionBox->SetupAttachment(RootComponent);
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

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ARPBaseInteractableObject::OnObjectOverlap);
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

void ARPBaseInteractableObject::OnObjectOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}
