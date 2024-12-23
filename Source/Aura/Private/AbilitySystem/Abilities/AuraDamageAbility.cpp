// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraDamageAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Types/AuraAbilityTypes.h"

void UAuraDamageAbility::MakeDamageEffectParams(FDamageEffectParams& OutParams, AActor* TargetActor) const
{
	OutParams.WorldContextObject = GetAvatarActorFromActorInfo();
	OutParams.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	OutParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	OutParams.DamageEffectClass = DamageEffectClass;
	OutParams.BaseDamage = GetDamageByLevel(GetAbilityLevel());
	OutParams.DamageTypeTag = DamageTypeTag;
	OutParams.AbilityLevel = GetAbilityLevel();

	// TODO : Knockback, Debuff, etc..
}

float UAuraDamageAbility::GetDamageByLevel(int32 Level) const
{
	return DamageCurve.GetValueAtLevel(Level);
}

void UAuraDamageAbility::PlayAttackMontage(UAnimMontage* MontageToPlay, bool bEndOnBlendOut)
{
	if (UAbilityTask_PlayMontageAndWait* AbilityTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), MontageToPlay, 1.f, NAME_None, false))
	{
		if (bEndOnBlendOut)
		{
			AbilityTask->OnBlendOut.AddDynamic(this, &ThisClass::FinishMontage);
		}
		AbilityTask->OnCompleted.AddDynamic(this, &ThisClass::FinishMontage);
		AbilityTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		AbilityTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		AbilityTask->ReadyForActivation();
	}
}

void UAuraDamageAbility::WaitGameplayEvent(const FGameplayTag& EventTag)
{
	// Wait gameplay event from attack montage
	if (UAbilityTask_WaitGameplayEvent* AbilityTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag, nullptr, true))
	{
		AbilityTask->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
		AbilityTask->ReadyForActivation();
	}
}

void UAuraDamageAbility::FinishMontage()
{
	bFinishMontage = true;
	TryEndAbility();
}

void UAuraDamageAbility::FinishAttack()
{
	bFinishAttack = true;
	TryEndAbility();
}

void UAuraDamageAbility::TryEndAbility()
{
	if (bFinishMontage && bFinishAttack)
	{
		K2_EndAbility();
	}
}
