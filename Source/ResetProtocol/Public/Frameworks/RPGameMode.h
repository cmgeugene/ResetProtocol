
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RPGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RESETPROTOCOL_API ARPGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	
	virtual void OnPostLogin(AController* NewPlayer) override;

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

	// 로그인 후 실행되는 함수
	UFUNCTION(BlueprintNativeEvent, Category = "RPGameMode")
	void PostLoginProcess(AController* NewPlayer);
	void PostLoginProcess_Implementation(AController* NewPlayer);

	
	virtual void Logout(AController* Exiting) override;

	// 로그아웃 후 실행되는 함수
	UFUNCTION(BlueprintNativeEvent, Category = "RPGameMode")
	void LogoutProcess(AController* Exiting);
	void LogoutProcess_Implementation(AController* Exiting);
	
	UFUNCTION(BlueprintCallable)
	TArray<AController*> GetPlayerControllerArray();

	// {UniqueNetID, CustomNickName}
	TMap<FString, FString> PlayerNickNames;

	
};
