// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RPLobbyGameMode.h"
#include "Frameworks/RPPlayerState.h"
#include "Frameworks/RPPlayerController.h"

ARPLobbyGameMode::ARPLobbyGameMode()
{
	bUseSeamlessTravel = true;

	PlayerStateClass = ARPPlayerState::StaticClass();
	PlayerControllerClass = ARPPlayerController::StaticClass();
}
