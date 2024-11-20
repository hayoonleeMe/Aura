// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Abilities의 Ability Class의 AbilitySpec을 생성해 GiveAbility를 수행하는 함수 
	void AddAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);

	/**
	 * Input Tag에 해당하는 Ability에 해당 Key의 Trigger Event를 발생시키는 함수
	 */
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
};
