// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_HitReact.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Interaction/CombatInterface.h"


void UAuraAbility_HitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(HitReactEffectClass);

	// GE_HitReact를 Owner에 적용
	const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(HitReactEffectClass);
	ActiveEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, CurrentActorInfo, ActivationInfo, EffectSpecHandle);

	// HitReactMontage를 재생하고 Wait
	if (UAnimMontage* HitReactMontage = ICombatInterface::Execute_GetHitReactMontage(GetAvatarActorFromActorInfo()))
	{
		if (UAbilityTask_PlayMontageAndWait* AbilityTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), HitReactMontage))
		{
			AbilityTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
			AbilityTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
			AbilityTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
			AbilityTask->ReadyForActivation();			
		}
	}
	
}

void UAuraAbility_HitReact::OnMontageFinished()
{
	// Owner에 적용했던 GE_HitReact 제거
	BP_RemoveGameplayEffectFromOwnerWithHandle(ActiveEffectHandle);
	K2_EndAbility();
}
