// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraProjectileAbility.h"
#include "AuraAbility_FireBall.generated.h"

/**
 * 폭발하는 큰 불덩이를 발사하고 충돌 시 작은 FireBolt를 주변으로 발사하는 FireBall Spell
 */
UCLASS()
class AURA_API UAuraAbility_FireBall : public UAuraProjectileAbility
{
	GENERATED_BODY()
	
public:
	UAuraAbility_FireBall();
	virtual FText GetDescription(int32 Level) const override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// UAbilityTask_TargetDataUnderMouse::TargetDataUnderMouseSetDelegate 델레게이트의 Callback 함수
	void OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	virtual void OnEventReceived(FGameplayEventData Payload) override;

private:
	FVector CachedImpactPoint;
	FName CachedCombatSocketName;
	
	// 레벨에 따른 발사할 FireBall 개수를 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBall")
	FScalableFloat NumFireBallsCurve;

	// Level에 해당하는 NumFireBalls 값 반환
	int32 GetNumFireBallsByLevel(float Level) const;

	// 레벨에 따른 발사할 EmberBolt 개수를 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBall")
	FScalableFloat NumEmberBoltsCurve;

	// Level에 해당하는 NumEmberBolts 값 반환
	int32 GetNumEmberBoltsByLevel(float Level) const;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage")
	FScalableFloat EmberBoltDamageCurve;

	// DamageCurve에서 Level에 따른 Damage Value를 반환한다.
	float GetEmberBoltDamageByLevel(int32 Level) const;

	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBall")
	TSubclassOf<AAuraProjectile> EmberBoltClass;

	// FireBall을 Target을 향해 발사한다.
	void SpawnFireBalls() const;
	
	// FireBall을 발사하는 방향을 계산할 때 사용할 퍼지는 정도를 나타내는 각도
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBall")
	float SpreadAngle;
};
