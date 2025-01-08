// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraAbility_HaloOfProtection.generated.h"

/**
 * 활성화 시 받는 Damage를 경감하는 Halo Of Protection Passive Spell
 */
UCLASS()
class AURA_API UAuraAbility_HaloOfProtection : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UAuraAbility_HaloOfProtection();
	virtual FText GetDescription(int32 Level) const override;

	// 현재 Ability Level에 따른 DamageReductionRate 값 반환
	float GetDamageReductionRate() const { return GetDamageReductionRateByLevel(GetAbilityLevel()); }

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	// 받는 Damage의 경감 비율을 저장하는 Curve 
	UPROPERTY(EditDefaultsOnly, Category="Aura|HaloOfProtection")
	FScalableFloat DamageReductionRateCurve;

	// Level에 따른 DamageReductionRate 값 반환
	float GetDamageReductionRateByLevel(float Level) const;
};
