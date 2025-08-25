// Fill out your copyright notice in the Description page of Project Settings.

#include "Manager/InteractableObject/RPSpawnPoint.h"

ARPSpawnPoint::ARPSpawnPoint() :
	MaxSpawnCount(1),
	Radius(50.0f),
	SpawnPointId("")
{
	bReplicates = false;			// 스폰 관련된 것은 클라이언트가 몰라도 됨
	SetReplicateMovement(false);
	SetActorEnableCollision(false);
}
