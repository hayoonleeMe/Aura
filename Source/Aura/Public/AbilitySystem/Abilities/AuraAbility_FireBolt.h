// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraProjectileAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraAbility_FireBolt.generated.h"

/**
 * Damage를 입히는 FireBall을 발사하는 FireBolt Spell
 */
UCLASS()
class AURA_API UAuraAbility_FireBolt : public UAuraProjectileAbility
{
	GENERATED_BODY()

public:
	virtual FText GetDescription(int32 Level) const override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// UAbilityTask_TargetDataUnderMouse::TargetDataUnderMouseSetDelegate 델레게이트의 Callback 함수
	void OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	virtual void OnEventReceived(FGameplayEventData Payload) override;

private:
	// UAbilityTask_TargetDataUnderMouse 결과 저장
	FVector CachedTargetLocation;

	FName CachedCombatSocketName;

	// FireBolt Spell의 레벨에 따른 발사할 FireBolt 개수를 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UCurveFloat> NumFireBoltsCurve;
};
