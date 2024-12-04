// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_HitReact.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Interaction/CombatInterface.h"


UAuraAbility_HitReact::UAuraAbility_HitReact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bRetriggerInstancedAbility = true;
}

void UAuraAbility_HitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// HitReactMontage를 재생하고 Wait
	if (UAnimMontage* HitReactMontage = ICombatInterface::Execute_GetHitReactMontage(GetAvatarActorFromActorInfo()))
	{
		if (UAbilityTask_PlayMontageAndWait* AbilityTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), HitReactMontage))
		{
			AbilityTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
			AbilityTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
			AbilityTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
			AbilityTask->ReadyForActivation();			
		}
	}
}
