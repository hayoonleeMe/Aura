// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

// Spell이 Unlock 된 이후로, Spell의 변경된 정보를 전달하는 델레게이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSpellAbilityChangedSignature, const FGameplayTag& /* SpellTag */, int32 /* SpellLevel */);

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

	// SpellTag를 가지는 Spell Ability가 Unlocked 또는 Equipped 상태인지를 반환
	bool IsSpellUnlocked(const FGameplayTag& SpellTag);

	// Spell Menu의 Spend Point Button을 눌렀을 때 작업을 수행
	UFUNCTION(Server, Reliable)
	void ServerSpendPointButtonPressed(const FGameplayTag& SpellTag, TSubclassOf<UGameplayAbility> SpellAbilityClass);


	FOnSpellAbilityChangedSignature OnSpellAbilityChangedDelegate;

private:
	// ============================================================================
	// Helper
	// ============================================================================

	// SpellTag를 가지는 Spell Ability Spec Pointer를 반환한다.
	// 존재하지 않으면 nullptr를 반환한다.
	FGameplayAbilitySpec* GetSpellSpecForSpellTag(const FGameplayTag& SpellTag);


	// ============================================================================
	// Spell
	// ============================================================================
	
	// SpellAbilityClass로 Ability Spec을 생성하고 SpellStatus_Unlocked Tag를 추가한 뒤 GiveAbility 수행
	void UnlockSpell(const FGameplayTag& SpellTag, const TSubclassOf<UGameplayAbility>& SpellAbilityClass);

	// SpellTag를 가지는 Spell Ability의 Level을 1만큼 증가
	void UpgradeSpell(const FGameplayTag& SpellTag);


	// 항상 서버에서 작업을 처리하므로 Client RPC를 이용해 기존 클라이언트에서 OnSpellAbilityChangedDelegate를 실행한다. 
	UFUNCTION(Client, Reliable)
	void ClientBroadcastSpellChange(const FGameplayTag& SpellTag, int32 SpellLevel);
};
