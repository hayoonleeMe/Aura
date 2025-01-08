// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraAbility_HealthSiphon.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UAuraAbility_HealthSiphon::UAuraAbility_HealthSiphon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UAuraAbility_HealthSiphon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (UAbilityTask_WaitGameplayEvent* WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FAuraGameplayTags::Get().Abilities_Passive_HealthSiphon))
	{
		WaitGameplayEvent->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
		WaitGameplayEvent->ReadyForActivation();
	}
}

void UAuraAbility_HealthSiphon::OnEventReceived(FGameplayEventData Payload)
{
	checkf(HealthRecoveryEffectClass, TEXT("Need to set HealthRecoveryEffectClass"));

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		// RecoveryAmount만큼 체력을 회복한다.
		const float DrainRate = GetDrainRateByLevel(GetAbilityLevel());
		const float RecoveryAmount = Payload.EventMagnitude /* Damage */ * DrainRate;
		
		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(HealthRecoveryEffectClass, 1.f, FGameplayEffectContextHandle());
		SpecHandle.Data->SetSetByCallerMagnitude(FAuraGameplayTags::Get().Attributes_Vital_Health, RecoveryAmount);
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
}

float UAuraAbility_HealthSiphon::GetDrainRateByLevel(float Level) const
{
	return DrainRateCurve.GetValueAtLevel(Level);
}
