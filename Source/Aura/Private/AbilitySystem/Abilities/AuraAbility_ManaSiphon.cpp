// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_ManaSiphon.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Interface/CombatInterface.h"

UAuraAbility_ManaSiphon::UAuraAbility_ManaSiphon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(AuraGameplayTags::Abilities_Passive_ManaSiphon);
	ActivationOwnedTags.AddTag(AuraGameplayTags::Abilities_Passive_ManaSiphon);
}

FText UAuraAbility_ManaSiphon::GetDescription(int32 Level) const
{
	const float DrainRate = GetDrainRateByLevel(Level);
	const int32 DrainPercent = DrainRate * 100.f;

	const FString RetStr = FString::Printf(TEXT(
		// Title
		"<Title>MANA SIPHON</>\n\n"

		// Level
		"<Default>Level: </><Level>%d</>\n"

		// Description
		"<Default>Restores </><Percent>%d%%</><Default> of the damage dealt as mana.</>"),

		// Values
		Level, DrainPercent
	);

	return FText::FromString(RetStr);
}

void UAuraAbility_ManaSiphon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (UAbilityTask_WaitGameplayEvent* WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, AuraGameplayTags::Abilities_Passive_ManaSiphon))
	{
		WaitGameplayEvent->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
		WaitGameplayEvent->ReadyForActivation();
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
		{
			CombatInterface->OnPassiveSpellActivated(AuraGameplayTags::Abilities_Passive_ManaSiphon);
		}
	}
}

void UAuraAbility_ManaSiphon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (HasAuthority(&CurrentActivationInfo))
	{
		if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
		{
			CombatInterface->OnPassiveSpellDeactivated(AuraGameplayTags::Abilities_Passive_ManaSiphon);
		}
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
		SpecHandle.Data->SetSetByCallerMagnitude(AuraGameplayTags::Attributes_Vital_Mana, RecoveryAmount);
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
}

float UAuraAbility_ManaSiphon::GetDrainRateByLevel(float Level) const
{
	return DrainRateCurve.GetValueAtLevel(Level);
}
