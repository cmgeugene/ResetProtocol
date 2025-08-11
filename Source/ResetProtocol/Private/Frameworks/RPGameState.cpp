// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RPGameState.h"
#include "Net/UnrealNetwork.h"

const TArray<APlayerState*>& ARPGameState::GetPlayerList() const
{
	return PlayerArray;
}

// [추가] 멀티캐스트 함수의 구현부
void ARPGameState::Multicast_OnPlayerListChanged_Implementation()
{
	// 이 함수가 모든 클라이언트에서 실행되면, 각 클라이언트의 OnPlayerListUpdated 델리게이트가 호출됩니다.
	OnPlayerListUpdated.Broadcast();
}
