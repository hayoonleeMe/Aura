// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraAbility_ManaSiphon.generated.h"

/**
 * 활성화 시 가하는 Damage의 일부만큼 마나를 회복하는 Mana Siphon Passive Spell
 * Must add Abilities.Passive.ManaSiphon tag to Activation Owned Tags
 */
UCLASS()
class AURA_API UAuraAbility_ManaSiphon : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UAuraAbility_ManaSiphon();
	virtual FText GetDescription(int32 Level) const override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
private:
	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);

	// 마나를 회복시키는 Effect
	UPROPERTY(EditDefaultsOnly, Category="Aura|ManaSiphon")
	TSubclassOf<UGameplayEffect> ManaRecoveryEffectClass;
	
	// 가한 Damage에서 회복할 마나의 비율을 저장하는 Curve 
	UPROPERTY(EditDefaultsOnly, Category="Aura|ManaSiphon")
	FScalableFloat DrainRateCurve;

	// Level에 따른 DrainRate 값 반환
	float GetDrainRateByLevel(float Level) const;
};
