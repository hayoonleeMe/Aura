// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UEnemyClassConfig;

/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	/*
	 *	Combat
	 */
	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
	TObjectPtr<UEnemyClassConfig> EnemyClassConfig;
};
