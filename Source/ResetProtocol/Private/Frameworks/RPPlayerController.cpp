// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RPPlayerController.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"
#include "Frameworks/RPPlayerState.h"
#include "Frameworks/RPGameState.h"
#include "Net/UnrealNetwork.h"
#include "Character/RPPlayerCharacter.h"
#include "Frameworks/RPGameMode.h"
#include "Component/RPMovableComponent.h"
#include "Component/Hologram/RPHologramComponent.h"

void ARPPlayerController::Server_OnResetSuccessHandle_Implementation(const EInteractObjectType Type)
{
	ARPGameState* RPGS = Cast<ARPGameState>(GetWorld()->GetGameState());
	RPGS->OnResetSuccess(Type);
}

void ARPPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void ARPPlayerController::RequestChangePlayerName(const FString& Name)
{
	Server_ChangePlayerName(Name);
}

void ARPPlayerController::Server_ChangePlayerName_Implementation(const FString& Name)
{
	if (PlayerState)
	{
		PlayerState->SetPlayerName(Name);
	}
}

bool ARPPlayerController::Server_ChangePlayerName_Validate(const FString& Name)
{
	if (Name.Len() > 0 && Name.Len() < 8)
	{
		return true;
	}
	return false;
}

void ARPPlayerController::OnRep_PlayerState()
{
	/*
	*  컨트롤러 생성 -> 컨트롤러에 플레이어 스테이트 할당 시 자동 호출
	*/
	
	Super::OnRep_PlayerState();

	bIsPlayerStateStandby = true;

	// 게임모드에서 InitializeSequence(); 실행
	// 혹은 다른 곳에서?

	PlayerStateProcess();
}

void ARPPlayerController::PlayerStateProcess_Implementation()
{

}

void ARPPlayerController::InitializeSequence()
{
	if (!bIsPlayerStateStandby)
	{
		UE_LOG(LogTemp, Error, TEXT("[RPPlayerController] InitializeSequence : 플레이어 스테이트 없음!"));
		return;
	}

	// 캐릭터 생성 & 빙의 -> OnPostLogin에 게임모드에서 명시적으로 실행
	// HUD 부착
}

void ARPPlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	if (IsLocalController())
	{
		Server_NotifyClientReady();
		UE_LOG(LogTemp, Display, TEXT("[SeamlessTravel] RPPlayerController::PostSeamlessTravel() called"));
	}
}

void ARPPlayerController::Server_NotifyClientReady_Implementation()
{
	if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode())
	{
		ARPGameMode* GM = Cast<ARPGameMode>(GameMode);
		if (GM)
		{
			GM->ControllerSeamlessComplete();
			UE_LOG(LogTemp, Display, TEXT("[SeamlessTravel] RPPlayerController::Server_NotifyClientReady() called"));
		}
	}

}

void ARPPlayerController::Client_ActivateHologram_Implementation(const AActor* GrabbedActor)
{
	if (!GrabbedActor)
	{
		return;
	}

	URPMovableComponent* MoveComp = GrabbedActor->FindComponentByClass<URPMovableComponent>();
	if (!MoveComp)
	{
		return;
	}

	if (MoveComp->HologramComp)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Client_ActivateHologram 호출"));
		MoveComp->HologramComp->ActivateHologram();
	}
}

void ARPPlayerController::Client_DeactivateHologram_Implementation(const AActor* GrabbedActor)
{
	if (!GrabbedActor)
	{
		return;
	}

	URPMovableComponent* MoveComp = GrabbedActor->FindComponentByClass<URPMovableComponent>();
	if (!MoveComp)
	{
		return;
	}

	if (MoveComp->HologramComp)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Client_DeactivateHologram 호출"));
		MoveComp->HologramComp->DeactivateHologram();
	}
}
