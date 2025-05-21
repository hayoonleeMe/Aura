// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraAbility_CastConfirmBase.h"
#include "AuraAbility_ArcaneSpike.generated.h"

/**
 * 땅에서 Arcane Shard가 튀어나와 일정 범위 내에 Damage를 입히는 Arcane Spike Spell
 */
UCLASS()
class AURA_API UAuraAbility_ArcaneSpike : public UAuraAbility_CastConfirmBase
{
	GENERATED_BODY()

public:
	UAuraAbility_ArcaneSpike();
	virtual FText GetDescription(int32 Level) const override;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override;
	virtual void OnEventReceived(FGameplayEventData Payload) override;

	virtual float GetRangeRadius() const override;

private:
	FVector CachedTargetLocation;

	void SpawnArcaneShard() const;

	// 하나의 Arcane Shard의 유효 범위
	static constexpr float ShardEffectiveRadius = 100.f;
	
	// ShardEffectiveRadius * ScaleRate
	float FinalShardEffectiveRadius = 0.f;

	// 커서 위치에서 FinalShardEffectiveRadius만큼 떨어진 곳에 Arcane Shard를 소환하는데, 각 Shard의 중심은 동일한 간격으로 배치된다. (ex. Shard가 3개면 120도씩)
	// 따라서 각 Shard를 모두 포함하는 최소 원의 반지름인 TotalEffectiveRadius = FinalShardEffectiveRadius * 2
	float TotalEffectiveRadius = 0.f;

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
