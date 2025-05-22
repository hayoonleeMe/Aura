#pragma once

#include "GameplayTagContainer.h"

#include "DamageEffectParams.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

/**
 *	Damage를 적용할 때 필요한 데이터를 저장
 */
USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams()
		: BaseDamage(0.f), AbilityLevel(0.f), DebuffChance(0.f), DebuffDuration(0.f)
	{ }

	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY()
	float BaseDamage;

	UPROPERTY()
	FGameplayTag DamageTypeTag;

	UPROPERTY()
	float AbilityLevel;

	UPROPERTY()
	FGameplayTag DebuffTag;

	UPROPERTY()
	float DebuffChance;

	UPROPERTY()
	float DebuffDuration;
};
