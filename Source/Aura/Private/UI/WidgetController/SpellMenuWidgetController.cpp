// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Data/SpellConfig.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	// SpellPoints 값 전달
	const AAuraPlayerState* AuraPS = GetAuraPlayerStateChecked();
	OnSpellPointsChangedDelegate.Broadcast(AuraPS->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	// Spell의 변경을 전달
	UAuraAbilitySystemComponent* AuraASC = GetAuraAbilitySystemComponentChecked();
	AuraASC->OnSpellAbilityChangedDelegate.AddWeakLambda(this, [this](const FGameplayTag& SpellTag, int32 SpellLevel)
	{
		if (SpellConfig)
		{
			FSpellInfo SpellInfo = SpellConfig->GetSpellInfoByTag(SpellTag);
			SpellInfo.SpellLevel = SpellLevel;
			OnSpellChangedDelegate.Broadcast(SpellInfo);
		}
	});

	// Spell의 장착 상황 변경을 전달
	AuraASC->OnEquippedSpellAbilityChangedDelegate.AddWeakLambda(this, [this](bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag)
	{
		if (SpellConfig)
		{
			const FSpellInfo SpellInfo = SpellConfig->GetSpellInfoByTag(SpellTag);
			OnEquippedSpellChangedDelegate.Broadcast(bEquipped, InputTag, SpellInfo);
		}
	});

	// 클라이언트에서 Spell을 Unlock할 때 Equip Button을 올바르게 활성화하기 위해 바인딩
	AuraASC->OnActivatableAbilitiesReplicatedDelegate.AddWeakLambda(this, [this]()
	{
		OnSpellGivenDelegate.Broadcast();
	});
	
	// SpellPoints가 변경되면 그 값을 전달
	AAuraPlayerState* AuraPS = GetAuraPlayerStateChecked();
	AuraPS->OnSpellPointsChangedDelegate.AddWeakLambda(this, [this](int32 Value)
	{
		OnSpellPointsChangedDelegate.Broadcast(Value);
	});
}

bool USpellMenuWidgetController::IsSelectedSpellOffensive() const
{
	return SelectedSpellTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Abilities.Offensive")));
}

bool USpellMenuWidgetController::IsSelectedSpellUnlocked() const
{
	UAuraAbilitySystemComponent* AuraASC = GetAuraAbilitySystemComponentChecked();
	return AuraASC->IsSpellUnlocked(SelectedSpellTag);
}

void USpellMenuWidgetController::SelectSpellGlobe(const FGameplayTag& SpellTag)
{
	if (SelectedSpellTag.MatchesTagExact(SpellTag))
	{
		// 선택했던 Spell Globe를 선택 해제하고 초기화
		SelectedSpellTag = FGameplayTag::EmptyTag;
		
		// 기존 Spell Globe 선택 해제 및 Sound 재생
		SelectSpellGlobeDelegate.Broadcast(false, SpellTag, true);
	}
	else
	{
		// 기존 Spell Globe 선택 해제
		SelectSpellGlobeDelegate.Broadcast(false, SelectedSpellTag, false);
		
		// 새 Spell Globe 선택
		SelectedSpellTag = SpellTag;
		SelectSpellGlobeDelegate.Broadcast(true, SelectedSpellTag, false);
	}
}

void USpellMenuWidgetController::DeselectSpellGlobe()
{
	if (SelectedSpellTag.IsValid())
	{
		const FGameplayTag DupSelectedSpellTag = SelectedSpellTag;
		SelectedSpellTag = FGameplayTag::EmptyTag;
		SelectSpellGlobeDelegate.Broadcast(false, DupSelectedSpellTag, false);
	}
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (!SelectedSpellTag.IsValid() || !SpellConfig)
	{
		return;
	}

	UAuraAbilitySystemComponent* AuraASC = GetAuraAbilitySystemComponentChecked();
	const FSpellInfo SpellInfo = SpellConfig->GetSpellInfoByTag(SelectedSpellTag);
	AuraASC->ServerSpendPointButtonPressed(SelectedSpellTag, SpellInfo.SpellAbilityClass);
}

void USpellMenuWidgetController::HandleEquipSpell(const FGameplayTag& InputTag)
{
	if (!SelectedSpellTag.IsValid() || !InputTag.IsValid())
	{
		return;
	}
	
	UAuraAbilitySystemComponent* AuraASC = GetAuraAbilitySystemComponentChecked();
	AuraASC->ServerHandleEquipSpell(SelectedSpellTag, InputTag);
}
