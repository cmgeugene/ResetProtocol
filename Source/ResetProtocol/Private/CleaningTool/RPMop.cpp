// Fill out your copyright notice in the Description page of Project Settings.


#include "CleaningTool/RPMop.h"

ARPMop::ARPMop()
{
	BottomMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BottomMeshComp"));
	BottomMeshComp->SetupAttachment(StaticMeshComp);
	BottomMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BottomMeshComp->SetSimulatePhysics(false);
	BottomMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BottomMeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
	BottomMeshComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	BottomMeshComp->SetIsReplicated(true);
}
