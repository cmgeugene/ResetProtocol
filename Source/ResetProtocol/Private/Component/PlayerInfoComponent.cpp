
#include "Component/PlayerInfoComponent.h"
#include "Net/UnrealNetwork.h"

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
	OnPlayerInfoComponentUpdated.Broadcast(Info);
}

void UPlayerInfoComponent::Server_SetPlayerInfo_Implementation(const FPlayerInfo& NewInfo)
{
	Info = NewInfo;

	if (GetOwnerRole() == ENetMode::NM_ListenServer)
	{
		OnRep_Info();
	}
}
