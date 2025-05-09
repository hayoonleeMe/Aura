// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

UAuraGameplayAbility::UAuraGameplayAbility()
{
	UnlockRequiredLevel = 1;
	MaxSpellLevel = 4;
}

FText UAuraGameplayAbility::GetLockedDescription() const
{
	return FText::FromString(FString::Printf(TEXT("<Default>Requires Level </><Level>%d </><Default>to unlock.</>"), UnlockRequiredLevel));
}

FText UAuraGameplayAbility::GetMaxLevelDescription()
{
	return FText::FromString(TEXT("<Default>Max Level Reached.</>"));
}

float UAuraGameplayAbility::GetManaCost(int32 Level) const
{
	float ManaCost = 0.f;
	if (UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (const FGameplayModifierInfo& ModifierInfo : CostEffect->Modifiers)
		{
			if (ModifierInfo.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				ModifierInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(Level, ManaCost);
				break;
			}
		}
	}
	return FMath::Abs(ManaCost);  
}

float UAuraGameplayAbility::GetCooldown(int32 Level) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(Level, Cooldown);
	}
	return Cooldown;
}
