// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_Debuff_Enfeeble.h"

#include "AbilitySystemComponent.h"

UAuraAbility_Debuff_Enfeeble::UAuraAbility_Debuff_Enfeeble()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UAuraAbility_Debuff_Enfeeble::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	checkf(DebuffEffectClass, TEXT("Need to set DebuffEffectClass"));
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo_Checked();
	
	const FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(DebuffEffectClass, 1.f, ASC->MakeEffectContext());
	ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);

	K2_EndAbility();
}
