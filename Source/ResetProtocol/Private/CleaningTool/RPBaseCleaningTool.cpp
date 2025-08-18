#include "CleaningTool/RPBaseCleaningTool.h"

ARPBaseCleaningTool::ARPBaseCleaningTool()
{
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

void ARPBaseCleaningTool::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARPBaseCleaningTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

