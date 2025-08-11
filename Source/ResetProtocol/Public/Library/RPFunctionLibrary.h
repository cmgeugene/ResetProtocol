#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FindSessionsCallbackProxy.h"
#include "Component/PlayerInfoComponent.h" // FPlayerInfo 사용을 위해 추가
#include "RPFunctionLibrary.generated.h"

class ARPPlayerState;
class APlayerController;

/**
 * 
 */
UCLASS()
class RESETPROTOCOL_API URPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	/*
	* FindSession, CreateSession 에서 반환하는 FOnlineSessionResult 에서 연결 가능한 IP : PORT 를 추출하는 함수
	* 연결 실패시 빈 문자열을 반환
	*/ 
	UFUNCTION(BlueprintPure, Category = "RPFunctionLibrary")
	static FString GetConnectionStringFromSessionResult(const FBlueprintSessionResult& SessionResult);

	/*
	* 지정된 URL(IP주소)로 클라이언트를 이동시키는 함수
	* PlayerController->ClientTravel() 의 래퍼
	*/
	UFUNCTION(BlueprintCallable, Category = "RPFunctionLibrary")
	static void BlueprintClientTravel(APlayerController* PC, const FString& URL);

	/*
	 * GetPlayerInfoFromController 함수를 이용하여
	 * 로컬 플레이어(자기 자신)의 PlayerInfo 구조체를 가져옴
	 */
	UFUNCTION(BlueprintPure, Category = "Reset Protocol|Player", meta = (WorldContext = "WorldContextObject"))
	static FPlayerInfo GetMyPlayerInfo(const UObject* WorldContextObject);

	/**
	 * 특정 PlayerController에 해당하는 PlayerInfo 구조체를 가져옴
	 */
	UFUNCTION(BlueprintPure, Category = "Reset Protocol|Player")
	static FPlayerInfo GetPlayerInfoFromController(APlayerController* PlayerController);
};
