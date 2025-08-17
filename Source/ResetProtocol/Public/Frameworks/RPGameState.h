
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "InteractableObject/RPBaseInteractableObject.h"
#include "RPGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerListUpdatedSignature);

UENUM(BlueprintType)
enum class EMatchPhase : uint8
{
	Lobby,
	InGame,
	Result
};

USTRUCT(BlueprintType)
struct FResetResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)		EInteractObjectType ObejctType;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)		int32 Count;

	// FindByKey를 위한 연산자 정의
	bool operator==(const EInteractObjectType InObejctType) const
	{
		return this->ObejctType == InObejctType;
	}
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

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "RPGameState")
	TArray<FResetResult> ResetProcessResult;

	UFUNCTION()
	void OnRep_MatchPhase();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "RPGameState")
	void SetMatchPhaseTo(const EMatchPhase NewPhase);

	UFUNCTION(BlueprintImplementableEvent, Category = "RPGameState")
	void OnRepMatchPhaseProcess();
	
	/*
	*	상호 작용 완료시 호출되는 함수
	*	결과 중 EInteractableObjectType이 이미 있으면 count 1 증가
	*	없으면 추가 후 count 1 증가
	*/
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "RPGameState")
	void OnResetSuccess(const EInteractObjectType Type);
	void OnResetSuccess_Implementation(const EInteractObjectType Type);

private:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
