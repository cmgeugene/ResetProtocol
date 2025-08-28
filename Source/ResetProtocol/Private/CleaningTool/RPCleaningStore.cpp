// Fill out your copyright notice in the Description page of Project Settings.


#include "CleaningTool/RPCleaningStore.h"

// Sets default values
ARPCleaningStore::ARPCleaningStore()
{
	PrimaryActorTick.bCanEverTick = true;

	PrimaryActorTick.bCanEverTick = false;
	bAlwaysRelevant = true;
	bReplicates = true;
	SetReplicateMovement(true);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(SceneRoot);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshComp->SetSimulatePhysics(true);
	StaticMeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
	StaticMeshComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	StaticMeshComp->SetIsReplicated(true);

}

// Called when the game starts or when spawned
void ARPCleaningStore::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARPCleaningStore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

