// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_ListenForModifyAttributeEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void UAuraAbility_ListenForModifyAttributeEvent::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(ModifyAttributeEventEffectClass);

	// 변경될 Attribute Tag 캐싱
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	AttributeToModifyTags.Add(GameplayTags.Attributes_Primary_Strength);
	AttributeToModifyTags.Add(GameplayTags.Attributes_Primary_Intelligence);
	AttributeToModifyTags.Add(GameplayTags.Attributes_Primary_Resilience);
	AttributeToModifyTags.Add(GameplayTags.Attributes_Primary_Vigor);

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

	// SetByCaller Magnitude가 설정되지 않으면 나타나는 Error Log를 방지하기 위해 변경되지 않는 다른 Attribute Tag에 대해서 0.f으로 설정
	for (const FGameplayTag& Tag : AttributeToModifyTags)
	{
		const bool bFound = Tag.MatchesTagExact(AttributeTag); 
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tag, bFound ? Payload.EventMagnitude : 0.f);
	}
	
	K2_ApplyGameplayEffectSpecToOwner(SpecHandle);
}
