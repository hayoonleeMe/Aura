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
	OnXPChangedDelegate.Broadcast(AuraAS->GetXP());
	OnLevelChangedDelegate.Broadcast(AuraAS->GetLevel());

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
	AuraASC->GetGameplayAttributeValueChangeDelegate(AuraAS->GetXPAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		OnXPChangedDelegate.Broadcast(Data.NewValue);
	});
	AuraASC->GetGameplayAttributeValueChangeDelegate(AuraAS->GetLevelAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		OnLevelChangedDelegate.Broadcast(Data.NewValue);
	});

	// Spell의 장착 상황 변경을 전달
	AuraASC->OnEquippedSpellAbilityChangedDelegate.AddUObject(this, &ThisClass::UpdateEquippedSpellChange);
}

void UOverlayWidgetController::UpdateEquippedSpellChange(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag) const
{
	if (SpellConfig)
	{
		const FSpellInfo SpellInfo = SpellConfig->GetSpellInfoByTag(SpellTag);
		OnEquippedSpellChangedDelegate.Broadcast(bEquipped, InputTag, SpellInfo, false); 
	}

	// Overlay에 Equipped Spell의 Cooldown Update
	UpdateEquippedSpellCooldown(bEquipped, InputTag, SpellTag);
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

void UOverlayWidgetController::UpdateEquippedSpellCooldown(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag) const
{
	UAuraAbilitySystemComponent* AuraASC = GetAuraAbilitySystemComponentChecked();
	if (const FGameplayAbilitySpec* SpellSpec = AuraASC->GetSpellSpecForSpellTag(SpellTag))
	{
		// Spell Cooldown Tag는 하나만 존재한다고 가정
		const FGameplayTagContainer* CooldownTags = SpellSpec->Ability->GetCooldownTags();
		const FGameplayTag SpellCooldownTag = CooldownTags ? CooldownTags->First() : FGameplayTag::EmptyTag;
		
		if (SpellCooldownTag.IsValid())
		{
			FOnGameplayEffectTagCountChanged& CooldownTagEvent = AuraASC->RegisterGameplayTagEvent(SpellCooldownTag, EGameplayTagEventType::NewOrRemoved);
		
			if (bEquipped)
			{
				// CooldownTag Event 등록
				CooldownTagEvent.AddWeakLambda(this, [this, InputTag, AuraASC](const FGameplayTag Tag, int32 Count)
				{
					if (Count > 0)
					{
						// Spell을 실행한 뒤 Cooldown이 적용되면, InputTag에 해당하는 EquippedGlobe Widget에서 Timer를 설정해 일정 시간마다 남은 Cooldown Time을 업데이트해 표시
						OnEquippedSpellCooldownStartDelegate.Broadcast(InputTag, AuraASC->GetCooldownTimeRemaining(Tag));
					}
					else
					{
						// Cooldown End
						OnEquippedSpellCooldownEndDelegate.Broadcast(InputTag);
					}
				});
			
				// Spell을 장착할 때 이미 Cooldown Tag가 존재하므로 바로 UI에 업데이트
				if (AuraASC->HasMatchingGameplayTag(SpellCooldownTag))
				{
					OnEquippedSpellCooldownStartDelegate.Broadcast(InputTag, AuraASC->GetCooldownTimeRemaining(SpellCooldownTag));
				}
			}
			else
			{
				// CooldownTag에 대해 등록된 Event 제거
				CooldownTagEvent.RemoveAll(this);
			}	
		}
	}
}
