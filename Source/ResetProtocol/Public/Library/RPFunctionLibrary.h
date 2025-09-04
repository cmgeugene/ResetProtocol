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

	// 에셋 썸네일 내보내기
	// 지정된 에셋의 썸네일을 생성하여 특정 경로에 저장합니다.
	// @param	InAsset		썸네일을 내보낼 에셋 오브젝트 레퍼런스
	// @param	InDirectoryPath	썸네일이 저장될 디렉토리의 전체 경로 (예: C:/Thumbnails)
	// @param	InFileName	썸네일이 저장될 파일 이름 (확장자 제외). 예: MyAssetThumbnail
	// @param	InResolution	내보낼 썸네일의 해상도 (예: X=256, Y=256).
	// @return	bool			썸네일 내보내기 성공 시 true, 실패 시 false를 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "ResetProtocol|FunctionLibrary")
	static bool ExportAssetThumbnail(UObject* InAsset, const FString& InDirectoryPath, const FString& InFileName, const FIntPoint& InResolution);
};
