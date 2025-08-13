// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RPGameMode.h"
#include "Frameworks/RPGameState.h"
#include "Frameworks/RPPlayerController.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Component/PlayerInfoComponent.h"
#include "BlueprintDataDefinitions.h"


void ARPGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	PostLoginProcess(NewPlayer);

	// 새 플레이어의 로그인 절차가 끝난 후, 모든 클라이언트에 목록을 갱신하라는 신호를 보냅니다.
	ARPGameState* GS = GetGameState<ARPGameState>();
	if (GS)
	{
		GS->Multicast_OnPlayerListChanged();
	}
}

FString ARPGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	UE_LOG(LogTemp, Warning, TEXT("[RPGameMode] InitNewPlayer raw Options received: %s"), *Options);
	
	// 반환값은 초기화 실패 시 오류 메세지를 나타냄, 빈 문자열 반환시 성공.
	FString ErroeMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	if (!ErroeMessage.IsEmpty()) return ErroeMessage;

	// 최초 접속 시 전달되는 닉네임 옵션을 파싱
	const FString CustomNickName = UGameplayStatics::ParseOption(Options, TEXT("NickName"));
	FString PlayerIdString;
	
	if (UniqueId.IsValid())
	{
		TSharedPtr<const FUniqueNetId> ActualUniqueNetId = UniqueId.GetUniqueNetId();
		if (ActualUniqueNetId.IsValid())
		{
			PlayerIdString = ActualUniqueNetId->ToString();
		}
	}

	// 재접속이 아닌 최초 접속이고, 닉네임이 유효할 경우에만 TMap에 추가
	if (!PlayerIdString.IsEmpty() && !CustomNickName.IsEmpty() && !PlayerNickNames.Contains(PlayerIdString))
	{
		PlayerNickNames.Add(PlayerIdString, CustomNickName);
		UE_LOG(LogTemp, Log, TEXT("[RPGameMode] New player's nickname cached: ID - %s , NickName - %s"), *PlayerIdString, *CustomNickName);
	}

	return TEXT("");
}

void ARPGameMode::PostLoginProcess_Implementation(AController* NewPlayer)
{
	UE_LOG(LogTemp, Display, TEXT("[RPGameMode] 로그인 완료 : %s"), *NewPlayer->GetName());

	//ARPPlayerState* RPPlayerState = NewPlayer->GetPlayerState<ARPPlayerState>();
	//if (RPPlayerState)
	//{
	//	const FString PlayerIdString = RPPlayerState->GetUniqueId().GetUniqueNetId()->ToString();
	//	FString* FoundNickName = PlayerNickNames.Find(PlayerIdString);

	//	// TMap 캐시에서 닉네임을 성공적으로 찾았을 경우
	//	if (FoundNickName != nullptr)
	//	{
	//		// PlayerState의 기본 PlayerName을 설정 (엔진 기본 기능 및 다른 시스템과 호환성을 위해 유지)
	//		RPPlayerState->SetPlayerName(*FoundNickName);

	//		// PlayerInfoComponent를 찾아 정보 설정
	//		UPlayerInfoComponent* InfoComponent = RPPlayerState->GetPlayerInfoComponent();
	//		if (InfoComponent)
	//		{
	//			FPlayerInfo NewInfo = InfoComponent->GetPlayerInfo(); // 기존 정보 가져오기
	//			NewInfo.PlayerName = FText::FromString(*FoundNickName);
	//			NewInfo.PS = RPPlayerState;
	//			NewInfo.PC = Cast<ARPPlayerController>(NewPlayer);
	//			
	//			InfoComponent->Info = NewInfo; // 최종 정보 설정

	//			// 리슨 서버의 경우 호스트 자신에게는 OnRep 함수가 자동 호출되지 않으므로 수동 호출
	//			if (GetNetMode() == NM_ListenServer)
	//			{
	//				InfoComponent->OnRep_Info();
	//			}
	//			UE_LOG(LogTemp, Log, TEXT("[RPGameMode] PlayerInfoComponent 업데이트 : UniqueNetID : %s , NickName : %s"), *PlayerIdString, *(*FoundNickName));
	//		}
	//	}
	//}
	
	/*
	*  블루프린트상에 RestartPlayer 구현해놓은 상태 : 캐릭터 생성 및 빙의 수행
	*/
}

void ARPGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	// 재접속 시 닉네임을 복원하기 위해 정보를 제거하지 않음
	UE_LOG(LogTemp, Warning, TEXT("[RPGameMode] Player %s logged out."), *Exiting->GetName());

	LogoutProcess(Exiting);

	// 플레이어가 나간 후, 모든 클라이언트에 목록을 갱신하라는 신호를 보냅니다.
	ARPGameState* GS = GetGameState<ARPGameState>();
	if (GS)
	{
		GS->Multicast_OnPlayerListChanged();
	}
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

void ARPGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	K2_OnHandleSeamlessTravelPlayer(C);
}

void ARPGameMode::ControllerSeamlessComplete()
{
	UE_LOG(LogTemp, Display, TEXT("[SeamlessTravel] RPGameMode : 현재 플레이어 수 %d"), CurrentSeamlessPlayer);
	CurrentSeamlessPlayer++;
	UE_LOG(LogTemp, Display, TEXT("[SeamlessTravel] RPGameMode : 현재 플레이어 수 %d"), CurrentSeamlessPlayer);
	ARPGameState* GS = Cast<ARPGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		int32 TotalPlayer = GS->PlayerArray.Num();
		UE_LOG(LogTemp, Display, TEXT("[SeamlessTravel] RPGameMode : 총 플레이어 수 %d"), TotalPlayer);
		if (TotalPlayer == CurrentSeamlessPlayer)
		{
			AllPlayerControllersTravelComplete();
			UE_LOG(LogTemp, Display, TEXT("[SeamlessTravel] RPGameMode::ControllerSeamlessComplete called"));
		}
	}
}

void ARPGameMode::AllPlayerControllersTravelComplete()
{
	//TArray<AController*> CurrentControllers = GetPlayerControllerArray();

	//if (!CurrentControllers.IsEmpty())
	//{
	//	for (AController* Controller : CurrentControllers)
	//	{
	//		APawn* NewPawn = SpawnDefaultPawnFor(Controller, FindPlayerStart(Controller));
	//		Controller->Possess(NewPawn);
	//		UE_LOG(LogTemp, Display, TEXT("[SeamlessTravel] RPGameMode::RestartAllPlayerControllers() called"));
	//	}
	//}
}


