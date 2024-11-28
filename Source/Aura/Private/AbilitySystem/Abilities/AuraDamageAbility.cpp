// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraDamageAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Types/AuraAbilityTypes.h"

void UAuraDamageAbility::MakeDamageEffectParams(FDamageEffectParams& OutParams, AActor* TargetActor) const
{
	OutParams.WorldContextObject = GetAvatarActorFromActorInfo();
	OutParams.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	OutParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	OutParams.DamageEffectClass = DamageEffectClass;
	OutParams.BaseDamage = GetDamageByLevel();
	OutParams.DamageTypeTag = DamageTypeTag;
	OutParams.AbilityLevel = GetAbilityLevel();

	// TODO : Knockback, Debuff, etc..
}

float UAuraDamageAbility::GetDamageByLevel() const
{
	return DamageCurve.GetValueAtLevel(GetAbilityLevel());
}
