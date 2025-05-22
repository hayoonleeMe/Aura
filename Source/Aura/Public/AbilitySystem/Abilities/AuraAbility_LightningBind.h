// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraAbility_CastConfirmBase.h"
#include "AuraAbility_LightningBind.generated.h"

/**
 * 일정 범위 내에 매 초마다 데미지를 입히고 Stun Debuff를 부여해 움직이지 못하게 하는 Lightning Bind Spell
 */
UCLASS()
class AURA_API UAuraAbility_LightningBind : public UAuraAbility_CastConfirmBase
{
	GENERATED_BODY()
	
public:
	UAuraAbility_LightningBind();
	
	virtual FText GetDescription(int32 Level) const override;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override;
	virtual void OnEventReceived(FGameplayEventData Payload) override;

	virtual float GetRangeRadius() const override;

private:
	// OnTargetDataUnderMouseSet()에서 저장되고, ComputeValidTargetLocation()를 통해 실제 유효한 위치로 보정해 사용한다.
	FVector CachedTargetLocation;

	void SpawnLightningBind();

	// 스펠 범위
	UPROPERTY(EditDefaultsOnly, Category="Aura|LightningBind")
	float EffectiveRadius;

	// EffectiveRadius * ScaleRate
	float FinalEffectiveRadius = 0.f;

	// 스펠 범위에 곱해질 Rate
	UPROPERTY(EditDefaultsOnly, Category="Aura|LightningBind")
	FScalableFloat ScaleRateCurve;

	// Level에 따른 ScaleRate 값 반환
	float GetScaleRateByLevel(float Level) const;

	// 데미지를 가하는 총 횟수
	UPROPERTY(EditDefaultsOnly, Category="Aura|LightningBind")
	FScalableFloat TotalDamageCountCurve;

	// Level에 따른 TotalDamageCount 값 반환
	float GetTotalDamageCountByLevel(float Level) const;
	
	// 데미지를 가하는 시간 간격 (초)
	UPROPERTY(EditDefaultsOnly, Category="Aura|LightningBind")
	float DamageInterval;
};
