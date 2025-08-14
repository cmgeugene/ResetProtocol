
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RPGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerListUpdatedSignature);

UENUM(BlueprintType)
enum class EMatchPhase : uint8
{
	Lobby,
	InGame,
	Result
};

/**
 * 
 */
UCLASS()
class RESETPROTOCOL_API ARPGameState : public AGameStateBase
{
	GENERATED_BODY()

public:


	// 기본 제공 PlayerArray에 대한 Getter
	UFUNCTION(BlueprintCallable, Category = "RPGameState")
	const TArray<APlayerState*>& GetPlayerList() const;



	UPROPERTY(ReplicatedUsing = OnRep_MatchPhase, BlueprintReadOnly, Category = "RPGameState")
	EMatchPhase MatchPhase;

	UFUNCTION()
	void OnRep_MatchPhase();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "RPGameState")
	void SetMatchPhaseTo(const EMatchPhase NewPhase);

	UFUNCTION(BlueprintImplementableEvent, Category = "RPGameState")
	void OnRepMatchPhaseProcess();

private:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
