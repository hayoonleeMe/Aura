// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_HealthSiphon.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Interface/CombatInterface.h"

UAuraAbility_HealthSiphon::UAuraAbility_HealthSiphon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

FText UAuraAbility_HealthSiphon::GetDescription(int32 Level) const
{
	const float DrainRate = GetDrainRateByLevel(Level);
	const int32 DrainPercent = DrainRate * 100.f;

	const FString RetStr = FString::Printf(TEXT(
		// Title
		"<Title>HEALTH SIPHON</>\n\n"

		// Description
		"<Default>Restores </><Percent>%d%%</><Default> of the damage dealt as health.</>"),

		// Values
		DrainPercent
	);

	return FText::FromString(RetStr);
}

void UAuraAbility_HealthSiphon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (UAbilityTask_WaitGameplayEvent* WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, AuraGameplayTags::Abilities_Passive_HealthSiphon))
	{
		WaitGameplayEvent->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
		WaitGameplayEvent->ReadyForActivation();
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
		{
			CombatInterface->OnPassiveSpellActivated(AuraGameplayTags::Abilities_Passive_HealthSiphon);
		}
	}
}

void UAuraAbility_HealthSiphon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (HasAuthority(&CurrentActivationInfo))
	{
		if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
		{
			CombatInterface->OnPassiveSpellDeactivated(AuraGameplayTags::Abilities_Passive_HealthSiphon);
		}
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
		SpecHandle.Data->SetSetByCallerMagnitude(AuraGameplayTags::Attributes_Vital_Health, RecoveryAmount);
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
}

float UAuraAbility_HealthSiphon::GetDrainRateByLevel(float Level) const
{
	return DrainRateCurve.GetValueAtLevel(Level);
}
