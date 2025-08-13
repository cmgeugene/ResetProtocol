
#include "Component/PlayerInfoComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"

// Sets default values for this component's properties
UPlayerInfoComponent::UPlayerInfoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UPlayerInfoComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UPlayerInfoComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerInfoComponent, Info);
}

void UPlayerInfoComponent::OnRep_Info()
{
	OnRepInfoProcess();
	OnPlayerInfoComponentUpdated.Broadcast(Info);
}

void UPlayerInfoComponent::OnRepInfoProcess_Implementation()
{

}

void UPlayerInfoComponent::Server_SetPlayerInfo_Implementation(const FText& NewNickname, bool bIsReady, int32 NewBytes)
{
	APlayerState* MyPlayerState = GetOwner<APlayerState>();
	APlayerController* MyPlayerController = MyPlayerState ? MyPlayerState->GetPlayerController() : nullptr;

	if (GetOwner()->HasAuthority())
	{
		FPlayerInfo NewInfo;

		NewInfo.PlayerName = NewNickname;    
		NewInfo.ReadyStatus = bIsReady;        
		NewInfo.Bytes = NewBytes;

		Info = NewInfo;

		OnRep_Info();
		UE_LOG(LogTemp, Display, TEXT("[PlayerInfoComponent] Server_SetPlayerInfo_Implementation : OnRep called"));
	}
}
