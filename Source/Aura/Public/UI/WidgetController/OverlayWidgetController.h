// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetController.h"
#include "SpellMenuWidgetController.h"
#include "OverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

// WBP_Overlay의 Menu Widget Switcher를 표시하거나 숨기는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShowMenuWidgetSwitcherSignature, bool, bVisible);

// InputTag에 등록된 Spell의 Cooldown이 시작됨을 알리는 델레게이트
// Spell의 Cooldown을 전달
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquippedSpellCooldownStartSignature, const FGameplayTag&, InputTag, float, Cooldown);

// InputTag에 등록된 Spell의 Cooldown이 끝남을 알리는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedSpellCooldownEndSignature, const FGameplayTag&, InputTag);

/**
 * Overlay Widget에서 사용할 WidgetController
 */
UCLASS()
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnAttributeChangedSignature OnHealthChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnAttributeChangedSignature OnMaxHealthChangedDelegate;
	
	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnAttributeChangedSignature OnManaChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnAttributeChangedSignature OnMaxManaChangedDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Aura|Overlay")
	FShowMenuWidgetSwitcherSignature ShowMenuWidgetSwitcherDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnEquippedSpellChangedSignature OnEquippedSpellChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnEquippedSpellCooldownStartSignature OnEquippedSpellCooldownStartDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnEquippedSpellCooldownEndSignature OnEquippedSpellCooldownEndDelegate;

private:
	// Spell의 장착 상태 변경을 업데이트
	void UpdateEquippedSpellChange(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag) const;

	// Startup Spell들을 업데이트
	void UpdateStartupSpells() const;

	// Equipped Spell의 Cooldown을 업데이트
	void UpdateEquippedSpellCooldown(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag) const;
};
