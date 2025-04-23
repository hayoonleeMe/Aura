// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_Debuff_Stun.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"

UAuraAbility_Debuff_Stun::UAuraAbility_Debuff_Stun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	AbilityTags.AddTag(AuraGameplayTags::Debuff_Stun);
	CancelAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_Offensive);
	CancelAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_EnemyAttack);
	CancelAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_ShamanSummonEnemy);
}

void UAuraAbility_Debuff_Stun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo_Checked();
	check(DebuffEffectClass);

	// Apply Debuff Effect
	const FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(DebuffEffectClass, 1.f, ASC->MakeEffectContext());
	ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);

	K2_EndAbility();
}
