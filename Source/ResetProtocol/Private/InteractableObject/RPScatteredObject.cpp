// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject/RPScatteredObject.h"
#include "Component/RPMovableComponent.h"

ARPScatteredObject::ARPScatteredObject()
{
	bReplicates = true;
	SetReplicateMovement(true);

	MoveComp = CreateDefaultSubobject<URPMovableComponent>(TEXT("MoveComp"));
	MoveComp->SetIsReplicated(true);

	ObjectType = EInteractObjectType::ScatteredObject;
}

void ARPScatteredObject::DragInteract_Implementation(AActor* Interactor)
{
	if (!MoveComp->GetIsHeld())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Grab On"));

		MoveComp->Server_Grab_Implementation(Interactor);

		// 테스트를 위해 3초뒤에 자동으로 떨어지도록
		FTimerHandle DropTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DropTimerHandle,
			[this, Interactor]()
			{
				IRPDragInterface::Execute_DropInteract(this, Interactor);
			},
			3.0f,
			false
		);
	}
}

void ARPScatteredObject::DropInteract_Implementation(AActor* Interactor)
{
	if (MoveComp->GetIsHeld())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Grab Off"));

		MoveComp->Server_Drop_Implementation();
	}
}

void ARPScatteredObject::OnObjectOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	IRPDragInterface::Execute_DragInteract(this, OtherActor);
}
