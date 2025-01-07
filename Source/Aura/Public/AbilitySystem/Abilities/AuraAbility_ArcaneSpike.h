// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageAbility.h"
#include "AuraAbility_ArcaneSpike.generated.h"

/**
 * 땅에서 Arcane Shard가 튀어나와 일정 범위 내에 Damage를 입히는 Arcane Spike Spell
 */
UCLASS()
class AURA_API UAuraAbility_ArcaneSpike : public UAuraDamageAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	// UAbilityTask_TargetDataUnderMouse::TargetDataUnderMouseSetDelegate 델레게이트의 Callback 함수
	void OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	virtual void OnEventReceived(FGameplayEventData Payload) override;

private:
	FVector CachedImpactPoint;

	void SpawnArcaneShard();
};
