// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraAbility_HitReact.generated.h"

/**
 * HitReact Effect를 적용하고 HitReact Montage를 재생하는 Ability
 */
UCLASS()
class AURA_API UAuraAbility_HitReact : public UAuraGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TSubclassOf<UGameplayEffect> HitReactEffectClass;

	// Owner에 적용한 GameplayEffect의 Handle
	FActiveGameplayEffectHandle ActiveEffectHandle;

	UFUNCTION()
	void OnMontageFinished();
};
