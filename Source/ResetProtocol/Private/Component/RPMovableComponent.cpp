// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/RPMovableComponent.h"
#include "InteractableObject/RPTrap.h"
#include "Camera/CameraComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Net/UnrealNetwork.h"

URPMovableComponent::URPMovableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	Holder = nullptr;
	GrabbedMesh = nullptr;
	bIsHeld = false;
}

void URPMovableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPMovableComponent, bIsHeld);
}

void URPMovableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner() || !GetOwner()->HasAuthority() || !bIsHeld || !Holder.IsValid())
	{
		return;
	}

	if (UPhysicsHandleComponent* GrabComp = (Holder.Get())->FindComponentByClass<UPhysicsHandleComponent>())
	{
		if (GrabComp->GetGrabbedComponent() == GrabbedMesh.Get())
		{
			if (USceneComponent* GrabAnchor = FindAnchor(Holder.Get()))
			{
				FVector NewLocation = GrabAnchor->GetComponentLocation() + GrabAnchor->GetForwardVector() * 150.0f;
				//FRotator NewRotation;

				GrabComp->SetTargetLocation(NewLocation);
			}
		}
	}
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

void URPMovableComponent::Server_Grab_Implementation(AActor* Interactor)
{
	// 캐릭터 앞에 붙이기
	if (!GetOwner())
	{
		return;
	}

	if (GetOwner()->HasAuthority()) 
	{
		Holder = Interactor;
		UMeshComponent* OwnerMesh = FindOwnerMeshComponent(GetOwner());
		if (!OwnerMesh)
		{
			return;
		}

		if (!IsPhysicsActive())
		{
			// Simulate Physics = false
			USceneComponent* Root = GetOwner()->GetRootComponent();
			if (!Root)
			{
				return;
			}

			UCameraComponent* CharacterCamera = Holder->FindComponentByClass<UCameraComponent>();
			if (CharacterCamera)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Grab : Physics Simulation Off"));

				Mulitcast_ApplyHoldCollision();
				Root->AttachToComponent(CharacterCamera, FAttachmentTransformRules::KeepWorldTransform);
				bIsHeld = true;
			}
		}
		else
		{
			// Simulate Physics = true
			FVector GrabLocation = OwnerMesh->GetComponentLocation();
			FRotator GrabRotation = OwnerMesh->GetComponentRotation();

			if (UPhysicsHandleComponent* GrabComp = Holder->FindComponentByClass<UPhysicsHandleComponent>())
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Grab : Physics Simulation Off On"));

				Mulitcast_ApplyHoldCollision();
				GrabComp->GrabComponentAtLocationWithRotation(OwnerMesh, NAME_None, GrabLocation, GrabRotation);
				GrabbedMesh = OwnerMesh;
				bIsHeld = true;
			}
		}
	}	
}

void URPMovableComponent::Server_Drop_Implementation()
{
	// 캐릭터에서 때기
	if (!GetOwner())
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		UMeshComponent* OwnerMesh = FindOwnerMeshComponent(GetOwner());
		if (!OwnerMesh)
		{
			return;
		}

		if (!IsPhysicsActive())
		{
			// Simulate Physics = false
			USceneComponent* Root = GetOwner()->GetRootComponent();
			if (!Root)
			{
				return;
			}

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Drop : Physics Simulation Off"));

			Root->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}
		else
		{
			// Simulate Physics = true
			if (Holder.IsValid())
			{
				if (UPhysicsHandleComponent* GrabComp = (Holder.Get())->FindComponentByClass<UPhysicsHandleComponent>())
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Drop : Physics Simulation On"));

					if (Cast<UMeshComponent>(GrabComp->GetGrabbedComponent()) == GrabbedMesh.Get())
					{
						GrabComp->ReleaseComponent();
					}
				}
			}
		}

		Multicast_RestoreCollision();

		bIsHeld = false;
		Holder = nullptr;
		GrabbedMesh = nullptr;
	}
}

void URPMovableComponent::Mulitcast_ApplyHoldCollision_Implementation()
{
	if (UMeshComponent* OwnerMesh = FindOwnerMeshComponent(GetOwner()))
	{
		OwnerMesh->SetCollisionProfileName("ObjectHold");
	}
}

void URPMovableComponent::Multicast_RestoreCollision_Implementation()
{
	if (UMeshComponent* OwnerMesh = FindOwnerMeshComponent(GetOwner()))
	{
		OwnerMesh->SetCollisionProfileName("PhysicsActor");
	}
}

void URPMovableComponent::OnPlaceComplete()
{
}

bool URPMovableComponent::IsPhysicsActive()
{
	if (UMeshComponent* OwnerMesh = FindOwnerMeshComponent(GetOwner()))
	{
		bool bSim = OwnerMesh->IsSimulatingPhysics();

		FString Msg = FString::Printf(TEXT("Mesh : %s, Physics : %s"), *OwnerMesh->GetName(), bSim ? TEXT("True") : TEXT("False"));

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, Msg);

		return bSim;
	}

	return false;
}

void URPMovableComponent::BeginPlay()
{
	Super::BeginPlay();

}

UMeshComponent* URPMovableComponent::FindOwnerMeshComponent(AActor* Owner) const
{
	if (!Owner)
	{
		return nullptr;
	}

	if (UStaticMeshComponent* StaticMesh = Owner->FindComponentByClass<UStaticMeshComponent>())
	{
		if (ARPTrap* TrapOwner = Cast<ARPTrap>(Owner))
		{
			StaticMesh = Cast<UStaticMeshComponent>(TrapOwner->ActiveMesh);
		}

		if (StaticMesh->GetStaticMesh())
		{
			return StaticMesh;
		}
	}
	
	if (USkeletalMeshComponent* SkeletalMesh = Owner->FindComponentByClass<USkeletalMeshComponent>())
	{
		if (SkeletalMesh->GetSkeletalMeshAsset())
		{
			return SkeletalMesh;
		}
	}
	
	return nullptr;
}

USceneComponent* URPMovableComponent::FindAnchor(AActor* Interactor) const
{
	if (!Interactor)
	{
		return nullptr;
	}

	if (UCameraComponent* CharacterCamera = Interactor->FindComponentByClass<UCameraComponent>())
	{
		return CharacterCamera;
	}

	return Interactor->GetRootComponent();
}
