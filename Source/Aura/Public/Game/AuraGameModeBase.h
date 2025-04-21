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

	// ============================================================================
	// Damage Calculation Coefficients
	// Damage 계산식에 사용되는 Level 별 계수
	// ============================================================================

	// ArmorPenetrationCoefficient 값을 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage Calculation Coefficients")
	FScalableFloat ArmorPenetrationCoefficientCurve;

	// EffectiveArmorCoefficient 값을 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage Calculation Coefficients")
	FScalableFloat EffectiveArmorCoefficientCurve;

	// CriticalHitResistanceCoefficient 값을 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage Calculation Coefficients")
	FScalableFloat CriticalHitResistanceCoefficientCurve;

	// StrengthDamageCoefficient 값을 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage Calculation Coefficients")
	FScalableFloat StrengthDamageCoefficientCurve;

	// IntelligenceDamageCoefficient 값을 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage Calculation Coefficients")
	FScalableFloat IntelligenceDamageCoefficientCurve;
};
