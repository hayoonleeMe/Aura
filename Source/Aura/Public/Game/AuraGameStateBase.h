// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AuraGameStateBase.generated.h"

class UAuraInputConfig;
class UInputMappingContext;

/**
 * 
 */
UCLASS()
class AURA_API AAuraGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AAuraGameStateBase();
	
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UAuraInputConfig> AuraInputConfig;

	// ============================================================================
	// Respawn
	// ============================================================================

	// 플레이어의 최대 목숨 개수
	UPROPERTY(EditDefaultsOnly, Category="Aura|Respawn")
	int32 TotalLifeCount;

	// 리스폰에 걸리는 시간
	UPROPERTY(EditDefaultsOnly, Category="Aura|Respawn")
	float RespawnTime;
};
