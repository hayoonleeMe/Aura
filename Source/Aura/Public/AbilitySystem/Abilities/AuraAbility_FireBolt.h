// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraProjectileAbility.h"
#include "AuraAbility_FireBolt.generated.h"

/**
 * Damage를 입히는 FireBall을 발사하는 FireBolt Spell
 */
UCLASS()
class AURA_API UAuraAbility_FireBolt : public UAuraProjectileAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// UAbilityTask_TargetDataUnderMouse::TargetDataUnderMouseSet 델레게이트의 Callback 함수
	void OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	// UAbilityTask_WaitGameplayEvent::EventReceived 델레게이트의 Callback 함수
	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);

private:
	// UAbilityTask_TargetDataUnderMouse 결과 저장
	FVector CachedTargetLocation;
};
