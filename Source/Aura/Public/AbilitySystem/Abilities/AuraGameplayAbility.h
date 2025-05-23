// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

// 스펠의 쿨다운 이벤트에 대한 델레게이트
// 쿨다운이 시작되면 양수 값을 전달하고, 끝나면 음수 값을 전달한다.
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSpellCooldownSignature, float /*Cooldown*/);

// 스펠의 스택 변경 이벤트에 대한 델레게이트
// 변경된 StackCount 값을 전달한다.
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSpellStackChangedSignature, int32 /*StackCount*/);

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UAuraGameplayAbility();
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	// 해당 어빌리티의 Cooldown Tag Change 콜백 함수
	void OnCooldownTagCountChanged(const FGameplayTag Tag, int32 Count);

	// 스펠이 쿨다운이 있다면 유효한 델레게이트 OnSpellCooldownDelegate를 반환하고, 아니라면 nullptr를 반환한다.
	FOnSpellCooldownSignature* GetOnSpellCooldownDelegate();
	FOnSpellCooldownSignature OnSpellCooldownDelegate;

	// true면 ETriggerEvent::Triggered로 실행되고, false면 ETriggerEvent::Started로 실행된다. 
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	uint8 bUseTriggeredEvent : 1;
	
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

	// ============================================================================
	// Spell Stack
	// ============================================================================

	// 스펠이 스택을 사용한다면 유효한 델레게이트 OnSpellStackChangedDelegate를 반환하고, 아니면 nullptr를 반환한다.
	FOnSpellStackChangedSignature* GetOnSpellStackChangedDelegate();
	FOnSpellStackChangedSignature OnSpellStackChangedDelegate;
	
	virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags) override;

	// 스펠이 스택을 사용하면 유효한 값을 반환하고, 아니라면 음수를 반환한다.
	int32 GetSpellStackCount() const;
	
protected:
	// CurrentStackCount를 업데이트하고, OnSpellStackChangedDelegate를 브로드캐스트한다. 
	void UpdateSpellStack(int32 AmountToAdd);

	// 스택을 사용하는지 여부
	UPROPERTY(EditDefaultsOnly, Category="Aura|Spell|Stack")
	uint8 bUseSpellStack : 1;
	
	// 최대 스택 수
	UPROPERTY(EditDefaultsOnly, Category="Aura|Spell|Stack", meta=(EditCondition="bUseSpellStack"))
	FScalableFloat MaxStackCountCurve;

	// Level에 따른 MaxStackCount 값을 반환한다.
	int32 GetMaxStackCountByLevel() const;

	// 현재 스택 수
	int32 CurrentStackCount = 0;
};
