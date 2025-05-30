// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class UAuraInputConfig;

// Spell이 Unlock 된 이후로, Spell의 변경된 정보를 전달하는 델레게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSpellAbilityChangedSignature, const FGameplayTag& /* SpellTag */);

// Spell의 장착 상태 변경을 전달하는 델레게이트
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnEquippedSpellAbilityChangedSignature, bool /* bEquipped */, const FGameplayTag& /* InputTag */, const FGameplayTag& /* SpellTag */);

// 서버에서 ActivatableAbilities가 변경되어 클라이언트에서 OnRep_ActivateAbilities이 호출될 때를 전달하는 델레게이트
DECLARE_MULTICAST_DELEGATE(FOnActivatableAbilitiesReplicatedSignature);

// Ability Spec Level 변경을 전달한다.
// UpgradeSpell()에서 레벨이 변경된다.
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilitySpecLevelChangedSignature, const FGameplayAbilitySpec& /*AbilitySpec*/);

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	virtual void OnRep_ActivateAbilities() override;

	bool IsInitialized() const { return bInitialized; }
	void SetInitialized() { bInitialized = true; }

	// ============================================================================
	// Ability
	// ============================================================================

	// AbilityFailedCallbacks 콜백 함수
	// Cost 또는 Cooldown 때문에 실패한 경우 화면에 위젯을 표시해 플레이어에게 알림
	void OnAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureTags);
	
	// Abilities의 Ability Class의 AbilitySpec을 생성해 GiveAbility를 수행하는 함수 
	void AddAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities, int32 InLevel);

	// InputID와 bUseTriggeredEvent에 해당하는 Ability를 실행하고 Pressed Event를 발생시키는 함수
	void AbilityInputPressed(int32 InputID, bool bUseTriggeredEvent);

	// InputID와 bUseTriggeredEvent에 해당하는 어빌리티를 찾아 FGameplayAbilitySpec*을 반환한다. 
	FGameplayAbilitySpec* FindAbilitySpecFromInputIDAndTriggerEvent(int32 InputID, bool bUseTriggeredEvent);

	// InputID에 해당하는 Ability의 Released Event를 발생시키는 함수
	void AbilityInputReleased(int32 InputID);

	// HitResultUnderMouse의 Hit Actor를 저장하는 Weak Ptr
	// Damage Ability (Offensive Spell) 에 의해 설정, 초기화
	TWeakObjectPtr<AActor> CursorTargetWeakPtr;

	// ============================================================================
	// Attribute
	// ============================================================================

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);
	
	// ============================================================================
	// Spell
	// ============================================================================

	// SpellTag를 가지는 Spell Ability가 Unlocked 또는 Equipped 상태인지를 반환
	bool IsSpellUnlocked(const FGameplayTag& SpellTag);

	// Spell Menu의 Spend Point Button을 눌렀을 때 작업을 수행
	UFUNCTION(Server, Reliable)
	void ServerSpendPointButtonPressed(const FGameplayTag& SpellTag, TSubclassOf<UGameplayAbility> SpellAbilityClass);

	// SpellTagToEquip를 InputTag가 나타내는 Input에 장착한다.
	// 만약 Input에 장착된 Spell을 다시 장착하려 하면 그 Spell을 장착 해제한다.
	UFUNCTION(Server, Reliable)
	void ServerHandleEquipSpell(const FGameplayTag& SpellTagToEquip, const FGameplayTag& InputTag);

	// 현재 실행할 수 있는 모든 Passive Spell을 실행한다.
	void ActivateAllPassiveSpells();

	FOnSpellAbilityChangedSignature OnSpellAbilityChangedDelegate;
	FOnEquippedSpellAbilityChangedSignature OnEquippedSpellAbilityChangedDelegate;
	FOnActivatableAbilitiesReplicatedSignature OnActivatableAbilitiesReplicatedDelegate;
	FOnAbilitySpecLevelChangedSignature OnAbilitySpecLevelChangedDelegate;

	// ============================================================================
	// Combat
	// ============================================================================

	// XP Attribute에 XPAmount를 더하는 GameplayEffect를 생성해 적용한다.
	void ApplyXPGainEffect(int32 XPAmount);

	// NonReplicated GameplayCue를 실행한다.
	UFUNCTION(Client, Reliable)
	void ClientExecuteGameplayCue(const FGameplayTag& CueTag);

	// ============================================================================
	// Helper
	// ============================================================================

	// 지정된 GameplayEffect 클래스를 기반으로 GameplayEffect Spec을 생성하고 적용한다.
	void ApplyEffectSpecToSelf(const TSubclassOf<UGameplayEffect>& EffectClass, float Level = 1.f);

	// 지정된 GameplayEffect 클래스를 기반으로 GameplayEffect Spec을 생성한 후,
	// DataTag와 Magnitude에 해당하는 SetByCaller Modifier를 설정하고 적용한다.
	void ApplyEffectSpecToSelfWithSetByCaller(const TSubclassOf<UGameplayEffect>& EffectClass, const FGameplayTag& DataTag, float Magnitude,  float Level = 1.f);

	// SpellTag를 가지는 Spell Ability Spec Pointer를 반환한다.
	// 존재하지 않으면 nullptr를 반환한다.
	FGameplayAbilitySpec* GetSpellSpecForSpellTag(const FGameplayTag& SpellTag);

	// Spell Ability의 Spell Tag를 반환한다.
	// 존재하지 않으면 EmptyTag를 반환한다.
	static FGameplayTag GetSpellTagForSpellSpec(const FGameplayAbilitySpec* SpellSpec);

	// ActivatableAbilities에 존재하는 모든 Spell Ability의 SpellTag와 InputTag 쌍을 반환한다.
	void GetSpellAndInputTagPairs(TArray<TTuple<FGameplayTag, FGameplayTag>>& OutArray);

	// CooldownTag를 가지는 Active Cooldown Effect의 가장 큰 Time Remaining을 반환
	float GetCooldownTimeRemaining(const FGameplayTag& CooldownTag) const;

private:
	// ASC가 초기화됐는지 여부
	bool bInitialized = false;

	UPROPERTY()
	TObjectPtr<UAuraInputConfig> AuraInputConfig;
	
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

	// 항상 서버에서 작업을 처리하므로 Client RPC를 이용해 기존 클라이언트에서 OnEquippedSpellAbilityChangedDelegate 실행한다. 
	UFUNCTION(Client, Reliable)
	void ClientBroadcastEquippedSpellChange(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag);
};
