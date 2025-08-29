// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/Hologram/RPPlacementSlot.h"
#include "InteractableObject/RPBaseInteractableObject.h"
#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"

ARPPlacementSlot::ARPPlacementSlot()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	RootPlacePoint = CreateDefaultSubobject<USceneComponent>(TEXT("SnapPoint"));
	SetRootComponent(RootPlacePoint);

	PlaceArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("PlaceArrow"));
	PlaceArrow->SetupAttachment(RootPlacePoint);
	PlaceArrow->ArrowSize = 0.5f;
	PlaceArrow->ArrowLength = 40.0f;
	PlaceArrow->ArrowColor = FColor::Green;			// 필요하면 바꿔도 됨
	PlaceArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	PlaceArrow->SetIsVisualizationComponent(true);	// 에디터 시각화 용도

	StaticHologram = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticHologram"));
	StaticHologram->SetupAttachment(RootPlacePoint);
	StaticHologram->SetIsReplicated(false);
	StaticHologram->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticHologram->SetCastShadow(false);
	StaticHologram->bReceivesDecals = false;
	StaticHologram->SetVisibility(false);

	SkeletalHologram = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalHologram"));
	SkeletalHologram->SetupAttachment(RootPlacePoint);
	SkeletalHologram->SetIsReplicated(false);
	SkeletalHologram->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalHologram->SetCastShadow(false);
	SkeletalHologram->bReceivesDecals = false;
	SkeletalHologram->SetVisibility(false);
	SkeletalHologram->bPauseAnims = true;
	SkeletalHologram->SetAnimationMode(EAnimationMode::AnimationSingleNode);

	CaptureRadius = 75.0f;
	HeightTolerance = 75.0f;
	YawAlignToleranceDegree = 25.0f;

	HologramMaterial = nullptr;

	bIsOccupied = false;
}

void ARPPlacementSlot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPPlacementSlot, bIsOccupied);
}

void ARPPlacementSlot::BeginPlay()
{
	Super::BeginPlay();

	if (!SlotTag.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No Tag!!"));
	}
}

UStaticMeshComponent* ARPPlacementSlot::FindActorStaticMesh(const AActor* Actor) const
{
	if (!Actor)
	{
		return nullptr;
	}

	TArray<UStaticMeshComponent*> StaticMeshes;
	Actor->GetComponents<UStaticMeshComponent>(StaticMeshes);
	for (UStaticMeshComponent* StaticMesh : StaticMeshes)
	{
		if (StaticMesh && StaticMesh->GetStaticMesh())
		{
			return StaticMesh;
		}
	}

	return nullptr;
}

USkeletalMeshComponent* ARPPlacementSlot::FindActorSkeletalMesh(const AActor* Actor) const
{
	if (!Actor)
	{
		return nullptr;
	}

	TArray<USkeletalMeshComponent*> SkeltalMeshes;
	Actor->GetComponents<USkeletalMeshComponent>(SkeltalMeshes);
	for (USkeletalMeshComponent* SkeltalMesh : SkeltalMeshes)
	{
		if (SkeltalMesh && SkeltalMesh->GetSkeletalMeshAsset())
		{
			return SkeltalMesh;
		}
	}

	return nullptr;
}

void ARPPlacementSlot::StaticMeshLocationAligned()
{
	if (!StaticHologram)
	{
		return;
	}

	FVector SlotLocation = GetActorLocation();

	// 2) 소스 메시의 월드 바운즈
	FBoxSphereBounds Bounds = StaticHologram->Bounds; // 월드 기준
	FVector Center = Bounds.Origin;
	FVector Half = Bounds.BoxExtent;

	// 3) 현재 "밑면"의 Z
	float BottomZ = Center.Z - Half.Z;
	float DeltaZ = SlotLocation.Z - BottomZ + 2.0f;		// 2.0f : 약간 띄울려고 사용

	FVector NewLocation = StaticHologram->GetComponentLocation();
	NewLocation.Z += DeltaZ;
	StaticHologram->SetWorldLocation(NewLocation);

	return;
}

void ARPPlacementSlot::SkeletalMeshLocationAligned()
{
	if (!SkeletalHologram)
	{
		return;
	}

	FVector SlotLocation = GetActorLocation();

	// Mesh의 World Bounds
	FBoxSphereBounds Bounds = SkeletalHologram->Bounds;		// 월드 기준
	FVector Center = Bounds.Origin;
	FVector Half = Bounds.BoxExtent;

	// 현재 밑면의 Z
	float BottomZ = Center.Z - Half.Z;
	float DeltaZ = SlotLocation.Z - BottomZ + 2.0f;			// 2.0f : 약간 띄울려고 사용

	FVector NewLocation = SkeletalHologram->GetComponentLocation();
	NewLocation.Z += DeltaZ;
	SkeletalHologram->SetWorldLocation(NewLocation);

	return;
}

void ARPPlacementSlot::ApplyGhostMaterial(UMeshComponent* Mesh)
{
	if (!Mesh || !HologramMaterial)
	{
		return;
	}

	const int32 NumOfMaterials = Mesh->GetNumMaterials();
	for (int32 i = 0; i < NumOfMaterials; i++)
	{
		Mesh->SetMaterial(i, HologramMaterial);
	}
}

bool ARPPlacementSlot::IsCompatibleForActor(const AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	const ARPBaseInteractableObject* GrabbedObject = Cast<ARPBaseInteractableObject>(Actor);
	if (GrabbedObject)
	{
		return GrabbedObject->HologramSlotTag == SlotTag;
	}

	return false;
}

void ARPPlacementSlot::ShowHologramFromActor(const AActor* GrabbedActor)
{
	if (UStaticMeshComponent* ActorStaticmesh = FindActorStaticMesh(GrabbedActor))
	{
		StaticHologram->SetStaticMesh(ActorStaticmesh->GetStaticMesh());
		StaticHologram->SetWorldTransform(GetActorTransform());
		StaticHologram->SetWorldScale3D(ActorStaticmesh->GetComponentScale());
		StaticMeshLocationAligned();

		ApplyGhostMaterial(StaticHologram);

		StaticHologram->SetVisibility(true);

		return;
	}
	
	if (USkeletalMeshComponent* ActorSkeletalMesh = FindActorSkeletalMesh(GrabbedActor))
	{
		SkeletalHologram->SetSkeletalMesh(ActorSkeletalMesh->GetSkeletalMeshAsset());
		SkeletalHologram->SetWorldTransform(GetActorTransform());
		SkeletalHologram->SetWorldScale3D(ActorSkeletalMesh->GetComponentScale());
		SkeletalMeshLocationAligned();

		ApplyGhostMaterial(SkeletalHologram);

		SkeletalHologram->SetVisibility(true);

		return;
	}

	return;
}

void ARPPlacementSlot::HideHologram()
{
	if (StaticHologram->GetStaticMesh())
	{
		StaticHologram->SetVisibility(false);
	}

	if (SkeletalHologram->GetSkeletalMeshAsset())
	{
		SkeletalHologram->SetVisibility(false);
	}
}

bool ARPPlacementSlot::IsInPlaceRangeAndScore(const AActor* Actor, const FVector& DropLocation, float& OutScore) const
{
	OutScore = TNumericLimits<float>::Max();
	if (!IsCompatibleForActor(Actor))
	{
		return false;
	}

	// 오브젝트를 바라봤을 떼 가로와 높이의 비율이 다름
	// - 오차를 3D Distance로 하면, 가로로는 괜찮은데 세로로는 영 이상한 곳이어도 붙을 수가 있음
	// => 가로와 높이를 나눠서 Score 계산
	const FVector SlotLoc = RootPlacePoint->GetComponentLocation();
	const float HorizontalDist = FVector::Dist2D(DropLocation, SlotLoc);
	if (HorizontalDist > CaptureRadius)
	{
		return false;
	}

	const float VerticalDelta = FMath::Abs(DropLocation.Z - SlotLoc.Z);
	if (VerticalDelta > HeightTolerance)
	{
		return false;
	}

	//if (YawAlignToleranceDegree > 0.f)
	//{
	//	const float SlotYaw = FMath::UnwindDegrees(RootPlacePoint->GetComponentRotation().Yaw);
	//	const float ActorYaw = FMath::UnwindDegrees(Actor ? Actor->GetActorRotation().Yaw : SlotYaw);
	//	const float YawDelta = FMath::Abs(FMath::FindDeltaAngleDegrees(ActorYaw, SlotYaw));
	//	if (YawDelta > YawAlignToleranceDegree)
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("YawDelta : %f"), YawDelta));
	//		return false;
	//	}
	//}
	
	// 단순 스코어: 수평 거리 + 수직 보정
	OutScore = HorizontalDist + VerticalDelta;

	return true;
}

bool ARPPlacementSlot::TryGetSnapTransform(const AActor* Actor, FTransform& OutTransform) const
{
	if (!IsCompatibleForActor(Actor))
	{
		return false;
	}

	OutTransform = GetActorTransform();

	return true;
}