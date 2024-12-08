// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_HitReact.h"

#include "AuraGameplayTags.h"
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
	const ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)
	{
		return;
	}
	
	const FTaggedCombatInfo TaggedCombatInfo = CombatInterface->GetTaggedCombatInfo(FAuraGameplayTags::Get().Abilities_HitReact);
	check(TaggedCombatInfo.AnimMontage);
	
	// HitReactMontage를 재생하고 Wait
	if (UAbilityTask_PlayMontageAndWait* AbilityTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), TaggedCombatInfo.AnimMontage))
	{
		AbilityTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		AbilityTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		AbilityTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		AbilityTask->ReadyForActivation();			
	}
}
