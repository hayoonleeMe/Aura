// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_HaloOfProtection.h"

UAuraAbility_HaloOfProtection::UAuraAbility_HaloOfProtection()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

float UAuraAbility_HaloOfProtection::GetDamageReductionRateByLevel(float Level) const
{
	return DamageReductionRateCurve.GetValueAtLevel(Level);
}
