// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraAbility_HealthSiphon.generated.h"

/**
 * 활성화 시 가하는 Damage의 일부만큼 체력을 회복하는 Health Siphon Passive Spell
 * Must add Abilities.Passive.HealthSiphon tag to Activation Owned Tags
 */
UCLASS()
class AURA_API UAuraAbility_HealthSiphon : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UAuraAbility_HealthSiphon();
	virtual FText GetDescription(int32 Level) const override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);

	// 체력을 회복시키는 Effect
	UPROPERTY(EditDefaultsOnly, Category="Aura|HealthSiphon")
	TSubclassOf<UGameplayEffect> HealthRecoveryEffectClass;
	
	// 가한 Damage에서 회복할 체력의 비율을 저장하는 Curve 
	UPROPERTY(EditDefaultsOnly, Category="Aura|HealthSiphon")
	FScalableFloat DrainRateCurve;

	// Level에 따른 DrainRate 값 반환
	float GetDrainRateByLevel(float Level) const;
};
