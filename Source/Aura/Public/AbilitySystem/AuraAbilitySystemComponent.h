// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

// Spell이 Unlock 된 이후로, Spell의 변경된 정보를 전달하는 델레게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSpellAbilityChangedSignature, const FGameplayTag& /* SpellTag */);

// Spell의 장착 상태 변경을 전달하는 델레게이트
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnEquippedSpellAbilityChangedSignature, bool /* bEquipped */, const FGameplayTag& /* InputTag */, const FGameplayTag& /* SpellTag */);

// 서버에서 ActivatableAbilities가 변경되어 클라이언트에서 OnRep_ActivateAbilities이 호출될 때를 전달하는 델레게이트
DECLARE_MULTICAST_DELEGATE(FOnActivatableAbilitiesReplicatedSignature);

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void OnRep_ActivateAbilities() override;
	
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

	// SpellTagToEquip를 InputTag가 나타내는 Input에 장착한다.
	// 만약 Input에 장착된 Spell을 다시 장착하려 하면 그 Spell을 장착 해제한다.
	UFUNCTION(Server, Reliable)
	void ServerHandleEquipSpell(const FGameplayTag& SpellTagToEquip, const FGameplayTag& InputTag);

	FOnSpellAbilityChangedSignature OnSpellAbilityChangedDelegate;
	FOnEquippedSpellAbilityChangedSignature OnEquippedSpellAbilityChangedDelegate;
	FOnActivatableAbilitiesReplicatedSignature OnActivatableAbilitiesReplicatedDelegate;

	// ============================================================================
	// Helper
	// ============================================================================

	// SpellTag를 가지는 Spell Ability Spec Pointer를 반환한다.
	// 존재하지 않으면 nullptr를 반환한다.
	FGameplayAbilitySpec* GetSpellSpecForSpellTag(const FGameplayTag& SpellTag);

	// InputTag가 추가된 Spell Ability Spec Pointer를 반환한다.
	// 존재하지 않으면 nullptr를 반환한다.
	FGameplayAbilitySpec* GetSpellSpecForInputTag(const FGameplayTag& InputTag);

	// Spell Ability가 장착 중인 InputTag를 반환한다.
	// 존재하지 않으면 EmptyTag를 반환한다.
	static FGameplayTag GetInputTagForSpellSpec(FGameplayAbilitySpec* SpellSpec);

	// Spell Ability의 Spell Tag를 반환한다.
	// 존재하지 않으면 EmptyTag를 반환한다.
	static FGameplayTag GetSpellTagForSpellSpec(const FGameplayAbilitySpec* SpellSpec);

	// ActivatableAbilities에 존재하는 모든 Spell Ability의 SpellTag와 InputTag 쌍을 반환한다.
	void GetSpellAndInputTagPairs(TArray<TTuple<FGameplayTag, FGameplayTag>>& OutArray);

private:
	// ============================================================================
	// Spell
	// ============================================================================
	
	// SpellAbilityClass로 Ability Spec을 생성하고 SpellStatus_Unlocked Tag를 추가한 뒤 GiveAbility 수행
	void UnlockSpell(const FGameplayTag& SpellTag, const TSubclassOf<UGameplayAbility>& SpellAbilityClass);

	// SpellTag를 가지는 Spell Ability의 Level을 1만큼 증가
	void UpgradeSpell(const FGameplayTag& SpellTag);

	// Spell을 장착 해제한다.
	// SpellSpecToUnEquip에서 InputTagToRemove Tag를 제거하고 SpellStatus_Equipped Tag를 추가한다.
	void UnEquipSpell(FGameplayAbilitySpec* SpellSpecToUnEquip, const FGameplayTag& InputTagToRemove, bool bClearPassiveSpell);

	// 항상 서버에서 작업을 처리하므로 Client RPC를 이용해 기존 클라이언트에서 OnSpellAbilityChangedDelegate를 실행한다. 
	UFUNCTION(Client, Reliable)
	void ClientBroadcastSpellChange(const FGameplayTag& SpellTag);

	// 항상 서버에서 작업을 처리하므로 Client RPC를 이용해 기존 클라이언트에서 OnEquippedSpellAbilityChangedDelegate 실행한다. 
	UFUNCTION(Client, Reliable)
	void ClientBroadcastEquippedSpellChange(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag);
};
