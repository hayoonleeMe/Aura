// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraAbility_ListenForModifyAttributeEvent.generated.h"

/**
 * 게임플레이 동안 Attribute를 변경하는 ModifyAttribute GameplayEvent를 기다리는 Ability
 */
UCLASS()
class AURA_API UAuraAbility_ListenForModifyAttributeEvent : public UAuraGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// Event Received Callback
	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);

	// AttributeTag로 등록된 SetByCaller Magnitude로 Attribute를 변경하는 GameplayEffect Class
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TSubclassOf<UGameplayEffect> ModifyAttributeEventEffectClass;

	// 변경될 Attribute Tag 캐싱
	UPROPERTY()
	TArray<FGameplayTag> AttributeToModifyTags;
};
