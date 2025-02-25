// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageAbility.h"
#include "AuraAbility_EnemyMelee.generated.h"

/**
 * Enemy의 근접 공격 Ability
 */
UCLASS(HideCategories=(Spell))
class AURA_API UAuraAbility_EnemyMelee : public UAuraDamageAbility
{
	GENERATED_BODY()

public:
	UAuraAbility_EnemyMelee();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void OnEventReceived(FGameplayEventData Payload) override;

	// 한번에 여러 대상을 공격할 수 있는지
	UPROPERTY(EditDefaultsOnly)
	uint8 bCanAttackMultiTarget : 1;

private:
	FName CachedCombatSocketName;
};
