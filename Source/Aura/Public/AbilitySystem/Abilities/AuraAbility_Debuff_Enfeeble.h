// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraAbility_Debuff_Enfeeble.generated.h"

/**
 * 10초 동안 대상이 주는 Damage의 20%를 감소시키는 Enfeeble Debuff
 */
UCLASS(HideCategories=("Aura|Input", "Aura|Spell"))
class AURA_API UAuraAbility_Debuff_Enfeeble : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UAuraAbility_Debuff_Enfeeble();

	static float GetDamageReductionRate() { return DamageReductionRate; }

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	// Damage 감소 비율
	static constexpr float DamageReductionRate = 0.2f;

	// Debuff의 효과를 나타내는 Effect Class 
	UPROPERTY(EditDefaultsOnly, Category="Aura|Debuff Enfeeble")
	TSubclassOf<UGameplayEffect> DebuffEffectClass;
};
