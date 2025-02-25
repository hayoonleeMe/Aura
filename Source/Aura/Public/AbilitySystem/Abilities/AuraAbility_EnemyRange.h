// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileAbility.h"
#include "AuraAbility_EnemyRange.generated.h"

/**
 * Enemy의 원거리 공격 Ability
 */
UCLASS(HideCategories=(Spell))
class AURA_API UAuraAbility_EnemyRange : public UAuraProjectileAbility
{
	GENERATED_BODY()

public:
	UAuraAbility_EnemyRange();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void OnEventReceived(FGameplayEventData Payload) override;

private:
	FVector CachedTargetLocation;
	FName CachedCombatSocketName;
};
