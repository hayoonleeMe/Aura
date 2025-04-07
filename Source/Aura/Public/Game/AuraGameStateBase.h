// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AuraGameStateBase.generated.h"

class UAttributeConfig;
class USpellConfig;
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
	TObjectPtr<UInputMappingContext> AbilityContext;

	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UInputMappingContext> CommonContext;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UInputMappingContext> UIContext;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UAuraInputConfig> AuraInputConfig;

	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<USpellConfig> SpellConfig;

	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UAttributeConfig> AttributeConfig;

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
