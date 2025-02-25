// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraAbility_HitReact.generated.h"

/**
 * HitReact Effect를 적용하고 HitReact Montage를 재생하는 Ability
 */
UCLASS(HideCategories=("Aura|Input", "Aura|Spell"))
class AURA_API UAuraAbility_HitReact : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UAuraAbility_HitReact();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
