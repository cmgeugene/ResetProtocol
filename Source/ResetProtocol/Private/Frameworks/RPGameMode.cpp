// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RPGameMode.h"
#include "Frameworks/RPGameState.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"


void ARPGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ARPGameState* RPGameState = GetGameState<ARPGameState>();
	if (RPGameState)
	{
		RPGameState->Multicast_OnPlayerListUpdated();
	}

	PostLoginProcess(NewPlayer);
}

FString ARPGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	// 반환값은 초기화 실패 시 오류 메세지를 나타냄, 빈 문자열 반환시 성공.
	FString ErroeMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	if (!ErroeMessage.IsEmpty()) return ErroeMessage;

	const FString CustomNickName = UGameplayStatics::ParseOption(Options, TEXT("NickName"));
	if (UniqueId.IsValid() && !CustomNickName.IsEmpty())
	{
		TSharedPtr<const FUniqueNetId> ActualUniqueNetId = UniqueId.GetUniqueNetId();
		if (ActualUniqueNetId.IsValid())
		{
			PlayerNickNames.Add(ActualUniqueNetId->ToString(), CustomNickName);
		}
		
	}

	return TEXT("");
}

void ARPGameMode::PostLoginProcess_Implementation(AController* NewPlayer)
{
	UE_LOG(LogTemp, Display, TEXT("[RPGameMode] 로그인 완료 : %s"), *NewPlayer->GetName());
	// 입장 UI 띄우기
	
	/*
	*  블루프린트상에 RestartPlayer 구현해놓은 상태 : 캐릭터 생성 및 빙의 수행
	*/

	if (NewPlayer && NewPlayer->PlayerState)
	{
		const FUniqueNetIdRepl& PlayerUniqueID = NewPlayer->PlayerState->GetUniqueId();

		if (PlayerUniqueID.IsValid())
		{
			TSharedPtr<const FUniqueNetId> ActualUniqueNetId = PlayerUniqueID.GetUniqueNetId();
			if (ActualUniqueNetId.IsValid())
			{
				FString* FoundNickName = PlayerNickNames.Find(ActualUniqueNetId->ToString());
				if (FoundNickName != nullptr)
				{
					NewPlayer->PlayerState->SetPlayerName(*FoundNickName);
				}
			}
		}
	}

}

void ARPGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ARPGameState* RPGameState = GetGameState<ARPGameState>();
	if (RPGameState)
	{
		RPGameState->Multicast_OnPlayerListUpdated();
	}
	
	if (Exiting && Exiting->PlayerState)
	{
		const FUniqueNetIdRepl& PlayerUniqueID = Exiting->PlayerState->GetUniqueId();
		if(PlayerUniqueID.IsValid())
		{
			TSharedPtr<const FUniqueNetId> ActualUniqueNetId = PlayerUniqueID.GetUniqueNetId();
			if (ActualUniqueNetId.IsValid())
			{
				PlayerNickNames.Remove(ActualUniqueNetId->ToString());
				UE_LOG(LogTemp, Warning, TEXT("[RPGameMode] Logout : Removed custom nickname for player %s on logout."), *ActualUniqueNetId->ToString());
			}
			
		}
	}

	LogoutProcess(Exiting);
}

void ARPGameMode::LogoutProcess_Implementation(AController* Exiting)
{
	UE_LOG(LogTemp, Display, TEXT("[RPGameMode] 로그아웃 완료 : %s"), *Exiting->GetName());
}

TArray<AController*> ARPGameMode::GetPlayerControllerArray()
{
	TArray<AController*> Controllers;
	UWorld* World = GetWorld();

	if (World)
	{
		for (TActorIterator<APlayerController> It(World); It; ++It)
		{
			APlayerController* PC = *It;
			if (PC)
			{
				Controllers.Add(PC);
			}
		}
	}

	return Controllers;
}
