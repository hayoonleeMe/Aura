// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_HaloOfProtection.h"

#include "AuraGameplayTags.h"
#include "Interface/CombatInterface.h"

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
		"<Percent>%d%%</>"),
		
		// Values
		DamageReductionPercent
	);

	return FText::FromString(RetStr);
}

void UAuraAbility_HaloOfProtection::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
		{
			CombatInterface->OnPassiveSpellActivated(AuraGameplayTags::Abilities_Passive_HaloOfProtection);
		}
	}
}

void UAuraAbility_HaloOfProtection::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (HasAuthority(&CurrentActivationInfo))
	{
		if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
		{
			CombatInterface->OnPassiveSpellDeactivated(AuraGameplayTags::Abilities_Passive_HaloOfProtection);
		}
	}
}

float UAuraAbility_HaloOfProtection::GetDamageReductionRateByLevel(float Level) const
{
	return DamageReductionRateCurve.GetValueAtLevel(Level);
}
