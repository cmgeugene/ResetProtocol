// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GlitchNoiseComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/RPPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "InteractableObject/RPBaseInteractableObject.h"

UGlitchNoiseComponent::UGlitchNoiseComponent()
{
	bAutoActivate = false;
}

void UGlitchNoiseComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = Cast<ARPBaseInteractableObject>(this->GetOwner());

	if (OwnerActor)
	{
		if (OwnerActor->bIsBug)
		{
			if (IsValid(OwnerActor->ActiveMesh))
			{
				OwnerActor->ActiveMesh->SetRenderCustomDepth(true);
				OwnerActor->ActiveMesh->SetCustomDepthStencilValue(1);
			}
		}
	}

	SoundRange = Cast<USphereComponent>(GetOwner()->GetComponentByClass(USphereComponent::StaticClass()));

	if (SoundRange)
	{
		SoundRange->OnComponentBeginOverlap.AddDynamic(this, &UGlitchNoiseComponent::OnOverlapBegin);
		SoundRange->OnComponentEndOverlap.AddDynamic(this, &UGlitchNoiseComponent::OnOverlapEnd);
	}
	else
	{
		
		UE_LOG(LogTemp, Warning, TEXT("[UGlitchNoiseComponent] : 스피어 콜리전 없음"), *GetOwner()->GetName());
	}
}

void UGlitchNoiseComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (OtherActor == PlayerCharacter)
	{
		this->Play(0.f);
	}
}

void UGlitchNoiseComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ARPPlayerCharacter* PlayerCharacter = Cast<ARPPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (OtherActor == PlayerCharacter)
	{
		this->FadeOut(0.5f, 0.0f);
	}
}
