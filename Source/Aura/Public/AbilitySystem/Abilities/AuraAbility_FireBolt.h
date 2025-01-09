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
	UAuraAbility_FireBolt();
	virtual FText GetDescription(int32 Level) const override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// UAbilityTask_TargetDataUnderMouse::TargetDataUnderMouseSetDelegate 델레게이트의 Callback 함수
	void OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	virtual void OnEventReceived(FGameplayEventData Payload) override;

private:
	FVector CachedImpactPoint;
	FName CachedCombatSocketName;

	// FireBolt Spell의 레벨에 따른 발사할 FireBolt 개수를 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBolt")
	FScalableFloat NumFireBoltsCurve;

	// Level에 해당하는 NumFireBolts 값 반환
	int32 GetNumFireBoltsByLevel(float Level) const;

	// 여러 개의 FireBolt를 Target을 향해 발사한다.
	void SpawnFireBolts() const;

	// FireBolt를 발사하는 방향을 계산할 때 사용할 퍼지는 정도를 나타내는 각도
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBolt")
	float SpreadAngle;

	// FireBolt의 HomingAccelerationMagnitude의 최소값
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBolt")
	float MinHomingAcceleration;

	// FireBolt의 HomingAccelerationMagnitude의 최대값
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBolt")
	float MaxHomingAcceleration;
};
