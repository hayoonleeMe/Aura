// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraAbility_Debuff_Ignite.generated.h"

/**
 * 4초 동안 매 초마다 활성화된 대상에게 Fire Type Damage를 입히는 Ignite Debuff
 * 이 Debuff를 유발한 Spell의 Damage의 10%에 해당하는 Damage를 매 초마다 주고, 중첩될 수 있다.
 */
UCLASS()
class AURA_API UAuraAbility_Debuff_Ignite : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UAuraAbility_Debuff_Ignite();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	// Ignite Damage를 입힐 때 사용할 GameplayEffect Class
	UPROPERTY(EditDefaultsOnly, Category="Aura|Debuff Ignite")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// Ignite Damage를 계산하기 위해 Incoming Damage에 곱할 비율
	static constexpr float DamageRate = 0.1f;
};
