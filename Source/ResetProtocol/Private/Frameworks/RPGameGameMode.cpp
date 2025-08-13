// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RPGameGameMode.h"
#include "Frameworks/RPPlayerState.h"
#include "Frameworks/RPPlayerController.h"

ARPGameGameMode::ARPGameGameMode()
{
	bUseSeamlessTravel = true;

	PlayerStateClass = ARPPlayerState::StaticClass();
	PlayerControllerClass = ARPPlayerController::StaticClass();
}
