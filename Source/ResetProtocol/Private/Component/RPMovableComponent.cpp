// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/RPMovableComponent.h"

URPMovableComponent::URPMovableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void URPMovableComponent::Server_Pickup_Implementation()
{
	FTimerHandle DestoryTimerHandle;
	if (AActor* OwnerActor = GetOwner())
	{
		TWeakObjectPtr<AActor> WeakOwnerActor = OwnerActor;

		GetWorld()->GetTimerManager().SetTimer(DestoryTimerHandle,
			[WeakOwnerActor]()
			{
				if (WeakOwnerActor.IsValid())
				{
					WeakOwnerActor->Destroy();
				}
			},
			0.5f,
			false
		);
	}
}

void URPMovableComponent::Server_StartDrag_Implementation()
{
	// 캐릭터 앞에 붙이기

}

void URPMovableComponent::Server_StartDrop_Implementation()
{
	// 캐릭터에서 때기+
}

void URPMovableComponent::OnPlaceComplete()
{
}

void URPMovableComponent::BeginPlay()
{
	Super::BeginPlay();

}
