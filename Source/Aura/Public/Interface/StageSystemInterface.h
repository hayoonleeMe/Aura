// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StageSystemInterface.generated.h"

class AAuraEnemy;

// This class does not need to be modified.
UINTERFACE()
class UStageSystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IStageSystemInterface
{
	GENERATED_BODY()

public:
	virtual void OnStageBeaconInteracted() {}
	virtual void RequestPlayerRespawn(APlayerController* PlayerController) {}
	virtual void RequestSpawnEnemy(const TSubclassOf<AAuraEnemy>& EnemyClass, FTransform SpawnTransform, bool bOverrideLocationZ) {}
};
