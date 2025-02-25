// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraAbility_EnemyRange.h"
#include "AuraAbility_RangeShaman.generated.h"

/**
 * Shaman Enemy의 Ranged Attack Ability
 */
UCLASS()
class AURA_API UAuraAbility_RangeShaman : public UAuraAbility_EnemyRange
{
	GENERATED_BODY()

public:
	UAuraAbility_RangeShaman();
};
