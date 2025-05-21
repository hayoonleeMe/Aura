// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraAbility_CastConfirmBase.h"
#include "AuraAbility_Firenado.generated.h"

class ARangeDecalActor;

/**
 * Fire Tornado를 소환해 일정 범위 내에 데미지를 입히는 Firenado Spell
 * 일정 범위 내에 일정 시간마다 데미지를 입히고 마지막에 큰 데미지를 입힌다.
 * 처음 실행하면 마우스 커서 위치에 스펠의 범위를 표시하고 원하는 위치에서 LMB를 입력하면 실제 데미지를 입히는 Firenado를 소환한다.
 * 처음 실행하고 다시 어빌리티 키를 입력하거나 다른 어빌리티를 실행하면 취소된다.
 */
UCLASS()
class AURA_API UAuraAbility_Firenado : public UAuraAbility_CastConfirmBase
{
	GENERATED_BODY()

public:
	UAuraAbility_Firenado();
	
	virtual FText GetDescription(int32 Level) const override;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override;
	virtual void OnEventReceived(FGameplayEventData Payload) override;

	virtual float GetRangeRadius() const override;

private:
	// 실제 Firenado를 소환할 위치
	// OnTargetDataUnderMouseSet()에서 저장되고, ComputeValidTargetLocation()를 통해 실제 유효한 위치로 보정해 사용한다.
	FVector CachedTargetLocation;

	// AFirenado Actor를 생성한다.
	void SpawnFirenado();

	// 데미지를 입히는 범위
	UPROPERTY(EditDefaultsOnly, Category="Aura|Firenado")
	float EffectiveRadius;

	// EffectiveRadius * ScaleRate
	float FinalEffectiveRadius = 0.f;

	// 마지막 폭발 데미지를 입히는 범위
	UPROPERTY(EditDefaultsOnly, Category="Aura|Firenado")
	float ExplosionEffectiveRadius;

	// ExplosionEffectiveRadius * ScaleRate
	float FinalExplosionEffectiveRadius = 0.f;

	// 데미지를 입히는 범위에 곱해질 Rate
	UPROPERTY(EditDefaultsOnly, Category="Aura|Firenado")
	FScalableFloat ScaleRateCurve;

	// Level에 따른 ScaleRate 값 반환
	float GetScaleRateByLevel(float Level) const;

	// DamageInterval마다 가해질 데미지는 UAuraDamageAbility::DamageCurve를 사용한다.
	// MakeDamageEffectParams()에서 자동으로 설정된다.

	// 마지막 데미지를 결정하기 위해 DamageInterval마다 가해지는 데미지에 곱해질 Rate
	UPROPERTY(EditDefaultsOnly, Category="Aura|Firenado")
	FScalableFloat FinalDamageRateCurve;

	// Level에 따른 ScaleRate 값 반환
	float GetFinalDamageRateByLevel(float Level) const;

	// 데미지를 가하는 시간 간격 (초)
	UPROPERTY(EditDefaultsOnly, Category="Aura|Firenado")
	float DamageInterval;

	// 데미지를 가하는 총 횟수
	UPROPERTY(EditDefaultsOnly, Category="Aura|Firenado")
	int32 TotalDamageCount;
};
