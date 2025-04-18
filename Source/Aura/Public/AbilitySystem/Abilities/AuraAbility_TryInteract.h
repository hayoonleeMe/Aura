// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraAbility_MoveAlongNavPathBase.h"
#include "AuraAbility_TryInteract.generated.h"

/**
 * Cursor로 선택한 TargetActor와의 상호작용을 시도하는 Ability
 * TargetActor와의 거리가 먼 경우, 충분히 가까워질 때까지 이동한 뒤 상호작용을 시도한다.
 */
UCLASS(HideCategories=("Aura|Spell"))
class AURA_API UAuraAbility_TryInteract : public UAuraAbility_MoveAlongNavPathBase
{
	GENERATED_BODY()

public:
	UAuraAbility_TryInteract();
	
protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;

	// TargetActor와 상호작용
	void Interact();

	// ArriveAcceptanceRadius의 기본값 캐싱 
	float InitialArriveAcceptanceRadius = 0.f;
};
