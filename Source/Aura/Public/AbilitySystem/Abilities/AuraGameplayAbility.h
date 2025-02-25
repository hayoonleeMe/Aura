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
	FGameplayTag StartupInputTag;

	// 해당 Spell을 Unlock할 수 있는 Character의 필요 레벨
	UPROPERTY(EditDefaultsOnly, Category="Aura|Spell")
	int32 UnlockRequiredLevel;

	// Spell의 최대 레벨
	UPROPERTY(EditDefaultsOnly, Category="Aura|Spell")
	int32 MaxSpellLevel;

	// Spell Menu의 Description Widget에 표시할 Level에 따른 Text를 반환한다.
	virtual FText GetDescription(int32 Level) const { return FText(); }

	// Locked Spell의 Description Text를 반환한다.
	FText GetLockedDescription() const;

	// Max Spell Level에 도달한 Spell의 Description Text를 반환한다.
	static FText GetMaxLevelDescription();

	// CostGameplayEffect의 Mana Attribute에 대한 Modifier의 Level에 따른 Magnitude를 반환한다.
	// 값을 구할 수 없으면 0.f를 반환한다.
	float GetManaCost(int32 Level) const;

	// CooldownGameplayEffect의 Duration의 Level에 따른 Magnitude를 반환한다.
	// 값을 구할 수 없으면 0.f를 반환한다.
	float GetCooldown(int32 Level) const;
};
