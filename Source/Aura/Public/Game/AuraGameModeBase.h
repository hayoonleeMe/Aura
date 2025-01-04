// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ScalableFloat.h"
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
	// ============================================================================
	// Combat
	// ============================================================================

	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
	TObjectPtr<UEnemyClassConfig> EnemyClassConfig;

	// Damage 계산식에 사용되는 계수 By Level
	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;

	// LevelUp에 필요한 XP 값을 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
	FScalableFloat XPRequirementCurve;

	// LevelUp 시 얻을 수 있는 Attribute Points 값을 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
	FScalableFloat AttributePointsAwardCurve;

	// LevelUp 시 얻을 수 있는 Spell Points 값을 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
    FScalableFloat SpellPointsAwardCurve;
};
