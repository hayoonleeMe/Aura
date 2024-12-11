// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_ListenForModifyAttributeEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void UAuraAbility_ListenForModifyAttributeEvent::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(ModifyAttributeEventEffectClass);

	// Wait ModifyAttribute Event 
	if (UAbilityTask_WaitGameplayEvent* AbilityTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FAuraGameplayTags::Get().Event_ModifyAttribute))
	{
		AbilityTask->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
		AbilityTask->ReadyForActivation();
	}
}

void UAuraAbility_ListenForModifyAttributeEvent::OnEventReceived(FGameplayEventData Payload)
{
	// SetByCaller 값으로 Attribute를 변경하는 GameplayEffect를 적용
	// AttributeTag는 Payload.TargetTags에 추가되어야 함
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(ModifyAttributeEventEffectClass);
	const FGameplayTag AttributeTag = Payload.TargetTags.GetByIndex(0);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, AttributeTag, Payload.EventMagnitude);
	K2_ApplyGameplayEffectSpecToOwner(SpecHandle);
}
