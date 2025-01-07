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

public:
	UAuraAbility_ArcaneSpike();
	virtual FText GetDescription(int32 Level) const override;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	// UAbilityTask_TargetDataUnderMouse::TargetDataUnderMouseSetDelegate 델레게이트의 Callback 함수
	void OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	virtual void OnEventReceived(FGameplayEventData Payload) override;

private:
	FVector CachedTargetLocation;

	void SpawnArcaneShard() const;

	// 하나의 Arcane Shard의 유효 범위
	static constexpr float EffectiveRadius = 100.f;
	
	// Spell의 최대 시전 사거리
	UPROPERTY(EditDefaultsOnly, Category="Aura|ArcaneSpike")
	float MaxCastRange;

	// EffectiveRadius에 곱해져 크기를 결정하는 ScaleRate를 저장하는 Curve 
	UPROPERTY(EditDefaultsOnly, Category="Aura|ArcaneSpike")
	FScalableFloat ScaleRateCurve;

	// Level에 따른 ScaleRate 값 반환
	float GetScaleRateByLevel(float Level) const;

	// 소환할 ArcaneShard 개수를 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|ArcaneSpike")
	FScalableFloat NumArcaneShardsCurve;

	// Level에 따른 NumArcaneShards 값 반환
	int32 GetNumArcaneShardsByLevel(float Level) const;
};
