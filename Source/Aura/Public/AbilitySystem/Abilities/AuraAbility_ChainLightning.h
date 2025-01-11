// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageAbility.h"
#include "AuraAbility_ChainLightning.generated.h"

/**
 * Target과 주변 적들에게 연쇄적으로 Damage를 입히는 Lightning Beam을 발사하는 ChainLightning Spell
 */
UCLASS()
class AURA_API UAuraAbility_ChainLightning : public UAuraDamageAbility
{
	GENERATED_BODY()

public:
	UAuraAbility_ChainLightning();
	virtual FText GetDescription(int32 Level) const override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// UAbilityTask_TargetDataUnderMouse::TargetDataUnderMouseSetDelegate 델레게이트의 Callback 함수
	void OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	virtual void OnEventReceived(FGameplayEventData Payload) override;

private:
	FVector CachedImpactPoint;
	FName CachedCombatSocketName;

	void CastLightningBeam() const;

	// Target을 결정할 때 Sweep을 수행할 FCollisionShape을 정의하기 위한 FVector 
	inline static FVector SweepBox = FVector(8.f, 8.f, 25.f);
	
	// Spell의 최대 시전 사거리
	UPROPERTY(EditDefaultsOnly, Category="Aura|ChainLightning")
	float MaxCastRange;

	// 번개가 첫 대상에 맞은 뒤 연쇄될 대상을 찾기 위해 사용되는 초기 연쇄 범위를 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|ChainLightning")
	FScalableFloat InitialChainRadiusCurve;

	// Level에 해당하는 InitialChainRadius 값 반환
	float GetInitialChainRadiusByLevel(float Level) const;

	// 번개의 최대 연쇄 횟수를 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|ChainLightning")
	FScalableFloat MaxChainCountCurve;

	// Level에 해당하는 MaxChainCount 값 반환
	int32 GetMaxChainCountByLevel(float Level) const;
	
	// 번개가 연쇄될 때 입히는 Damage 비율을 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|ChainLightning")
	FScalableFloat ChainDamageRateCurve;

	// Level에 해당하는 ChainDamageRate 값 반환
	float GetChainDamageRateByLevel(float Level) const;
};
