// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UAuraGameplayAbility();
	
	// Ability를 활성화하는 Input Key를 나타내는 Tag
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	FGameplayTag InputTag;

	// 해당 Spell을 Unlock할 수 있는 필요 레벨
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	int32 LevelRequirement;

};
