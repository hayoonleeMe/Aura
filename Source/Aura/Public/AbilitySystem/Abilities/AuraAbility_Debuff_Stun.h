// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraAbility_Debuff_Stun.generated.h"

/**
 * 일정 시간 동안 대상이 아무런 동작도 할 수 없게 기절시키는 Stun Debuff
 */
UCLASS()
class AURA_API UAuraAbility_Debuff_Stun : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UAuraAbility_Debuff_Stun();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Aura|Debuff Stun")
	TSubclassOf<UGameplayEffect> DebuffEffectClass;
};
