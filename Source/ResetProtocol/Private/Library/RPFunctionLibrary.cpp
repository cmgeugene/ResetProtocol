// Fill out your copyright notice in the Description page of Project Settings.


#include "Library/RPFunctionLibrary.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Frameworks/RPPlayerState.h"

FString URPFunctionLibrary::GetConnectionStringFromSessionResult(const FBlueprintSessionResult& SessionResult)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface)
        {
            FString ConnectString;
            if (SessionInterface->GetResolvedConnectString(SessionResult.OnlineResult, NAME_GamePort, ConnectString))
            {
                return ConnectString;
            }
        }
    }
    // 실패한 경우 빈 문자열을 반환
    return FString();
}

void URPFunctionLibrary::BlueprintClientTravel(APlayerController* PC, const FString& URL)
{
    if (PC)
    {
        PC->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[RPFunctionLibrary] BlueprintClientTravel : 플레이어 컨트롤러 없음"));
    }
}

FPlayerInfo URPFunctionLibrary::GetMyPlayerInfo(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
		return GetPlayerInfoFromController(PC);
	}
	return FPlayerInfo();
}

FPlayerInfo URPFunctionLibrary::GetPlayerInfoFromController(APlayerController* PlayerController)
{
	if (PlayerController)
	{
		ARPPlayerState* RPPlayerState = PlayerController->GetPlayerState<ARPPlayerState>();
		if (RPPlayerState)
		{
			UPlayerInfoComponent* InfoComponent = RPPlayerState->GetPlayerInfoComponent();
			if (InfoComponent)
			{
				return InfoComponent->GetPlayerInfo();
			}
		}
	}
	return FPlayerInfo();
}
