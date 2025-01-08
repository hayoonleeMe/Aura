// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_ManaSiphon.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UAuraAbility_ManaSiphon::UAuraAbility_ManaSiphon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UAuraAbility_ManaSiphon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (UAbilityTask_WaitGameplayEvent* WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FAuraGameplayTags::Get().Abilities_Passive_ManaSiphon))
	{
		WaitGameplayEvent->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
		WaitGameplayEvent->ReadyForActivation();
	}
}

void UAuraAbility_ManaSiphon::OnEventReceived(FGameplayEventData Payload)
{
	checkf(ManaRecoveryEffectClass, TEXT("Need to set ManaRecoveryEffectClass"));

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		// RecoveryAmount만큼 마나를 회복한다.
		const float DrainRate = GetDrainRateByLevel(GetAbilityLevel());
		const float RecoveryAmount = Payload.EventMagnitude /* Damage */ * DrainRate;
		
		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ManaRecoveryEffectClass, 1.f, FGameplayEffectContextHandle());
		SpecHandle.Data->SetSetByCallerMagnitude(FAuraGameplayTags::Get().Attributes_Vital_Mana, RecoveryAmount);
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
}

float UAuraAbility_ManaSiphon::GetDrainRateByLevel(float Level) const
{
	return DrainRateCurve.GetValueAtLevel(Level);
}
