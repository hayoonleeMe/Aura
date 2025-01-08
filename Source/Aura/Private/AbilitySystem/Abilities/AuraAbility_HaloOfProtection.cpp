// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_HaloOfProtection.h"

UAuraAbility_HaloOfProtection::UAuraAbility_HaloOfProtection()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

FText UAuraAbility_HaloOfProtection::GetDescription(int32 Level) const
{
	const float DamageReductionRate = GetDamageReductionRateByLevel(Level);
	const int32 DamageReductionPercent = DamageReductionRate * 100.f;

	const FString RetStr = FString::Printf(TEXT(
		// Title
		"<Title>HALO OF PROTECTION</>\n\n"

		// Description
		"<Default>Reduces incoming damage by </>"
		"<Percent>%d%%</><Default>.</>"),
		
		// Values
		DamageReductionPercent
	);

	return FText::FromString(RetStr);
}

float UAuraAbility_HaloOfProtection::GetDamageReductionRateByLevel(float Level) const
{
	return DamageReductionRateCurve.GetValueAtLevel(Level);
}
