#include "CleaningTool/RPBaseCleaningTool.h"
#include "Net/UnrealNetwork.h"

ARPBaseCleaningTool::ARPBaseCleaningTool()
{
	PrimaryActorTick.bCanEverTick = false;
	bAlwaysRelevant = true;
	bReplicates = true;
	SetReplicateMovement(true);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
//	SetRootComponent(StaticMeshComp);
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

	ePurchaseState = EPurchaseState::NotPurchased;
	
}

void ARPBaseCleaningTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARPBaseCleaningTool::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPBaseCleaningTool, ePurchaseState);
	DOREPLIFETIME(ARPBaseCleaningTool, eCleaningToolState);
}

