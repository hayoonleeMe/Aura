// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraDamageAbility.generated.h"

struct FDamageEffectParams;

/**
 * Damage를 입히는 GameplayAbility
 */
UCLASS()
class AURA_API UAuraDamageAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MakeDamageEffectParams(FDamageEffectParams& OutParams, AActor* TargetActor) const;

	// DamageCurve에서 Level에 따른 Damage Value를 반환한다.
	float GetDamageByLevel() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage")
	FGameplayTag DamageTypeTag;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage")
	FScalableFloat DamageCurve;

	// MontageToPlay를 재생하고 종료되면 EndAbility
	void PlayAttackMontage(UAnimMontage* MontageToPlay, bool bEndOnBlendOut);

	// EventTag에 해당하는 GameplayEvent를 받으면 공격 작업을 수행하고 EndAbility
	void WaitGameplayEvent(const FGameplayTag& EventTag); 

	// Montage 재생을 끝내고 TryEndAbility
	UFUNCTION()
	void FinishMontage();
	
	// Attack Anim Montage가 종료됐는지
	uint8 bFinishMontage : 1;
	
	// UAbilityTask_WaitGameplayEvent::EventReceived 델레게이트 Callback 함수
	UFUNCTION()
	virtual void OnEventReceived(FGameplayEventData Payload) {}

	// 공격 작업을 끝내고 TryEndAbility
	void FinishAttack();

	// GameplayEvent를 Receive하고 공격 작업을 모두 수행했는지
	uint8 bFinishAttack : 1;

	// Montage 재생과 GameplayEvent를 받은 후 공격 작업을 모두 수행해야 EndAbility
	void TryEndAbility();
};
