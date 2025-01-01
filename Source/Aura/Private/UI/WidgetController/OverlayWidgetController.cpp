// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Data/SpellConfig.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAS = GetAuraAttributeSetChecked();
	
	OnHealthChangedDelegate.Broadcast(AuraAS->GetHealth());
	OnMaxHealthChangedDelegate.Broadcast(AuraAS->GetMaxHealth());
	OnManaChangedDelegate.Broadcast(AuraAS->GetMana());
	OnMaxManaChangedDelegate.Broadcast(AuraAS->GetMaxMana());

	UpdateStartupSpells();
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAS = GetAuraAttributeSetChecked();
	UAuraAbilitySystemComponent* AuraASC = GetAuraAbilitySystemComponentChecked();

	// Attribute가 변경되면 Widget으로 새 데이터 전송
	AuraASC->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		OnHealthChangedDelegate.Broadcast(Data.NewValue);
	});
	AuraASC->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		OnMaxHealthChangedDelegate.Broadcast(Data.NewValue);
	});
	AuraASC->GetGameplayAttributeValueChangeDelegate(AuraAS->GetManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		OnManaChangedDelegate.Broadcast(Data.NewValue);
	});
	AuraASC->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		OnMaxManaChangedDelegate.Broadcast(Data.NewValue);
	});

	// Spell의 장착 상황 변경을 전달
	AuraASC->OnEquippedSpellAbilityChangedDelegate.AddUObject(this, &ThisClass::UpdateEquippedSpellChange);
}

void UOverlayWidgetController::UpdateEquippedSpellChange(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag) const
{
	if (SpellConfig)
	{
		const FSpellInfo SpellInfo = SpellConfig->GetSpellInfoByTag(SpellTag);
		OnEquippedSpellChangedDelegate.Broadcast(bEquipped, InputTag, SpellInfo);
	}	
}

void UOverlayWidgetController::UpdateStartupSpells() const
{
	UAuraAbilitySystemComponent* AuraASC = GetAuraAbilitySystemComponentChecked();
	
	TArray<TTuple<FGameplayTag, FGameplayTag>> StartupSpells;
	AuraASC->GetSpellAndInputTagPairs(StartupSpells);
	for (const TTuple<FGameplayTag, FGameplayTag>& Tuple : StartupSpells)
	{
		UpdateEquippedSpellChange(true, Tuple.Value, Tuple.Key);
	}
}
