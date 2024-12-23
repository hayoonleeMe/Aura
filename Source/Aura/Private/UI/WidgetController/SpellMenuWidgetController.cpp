// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Data/SpellConfig.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	// SpellPoints 값 전달
	const AAuraPlayerState* AuraPS = GetAuraPlayerStateChecked();
	UpdateSpellPoints(AuraPS->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	// Spell의 변경을 전달
	UAuraAbilitySystemComponent* AuraASC = GetAuraAbilitySystemComponentChecked();
	AuraASC->OnSpellAbilityChangedDelegate.AddWeakLambda(this, [this](const FGameplayTag& SpellTag)
	{
		if (SpellConfig)
		{
			OnSpellChangedDelegate.Broadcast(SpellConfig->GetSpellInfoByTag(SpellTag));

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
	AuraPS->OnSpellPointsChangedDelegate.AddUObject(this, &ThisClass::UpdateSpellPoints);
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

bool USpellMenuWidgetController::CanSpendPoint() const
{
	// Spell Point가 없거나 선택한 Spell Globe가 없으므로 불가능
	if (!bHasSpellPoints || !SelectedSpellTag.IsValid())
	{
		return false;
	}

	// Spell이 이미 Unlock된 상태라면 항상 가능
	UAuraAbilitySystemComponent* AuraASC = GetAuraAbilitySystemComponentChecked();
	if (AuraASC->IsSpellUnlocked(SelectedSpellTag))
	{
		return true;
	}

	// 현재 플레이어의 레벨이 Spell을 Unlock 할 수 있는 레벨인지 반환
	if (SpellConfig)
	{
		if (const TSubclassOf<UGameplayAbility>& SpellAbilityClass = SpellConfig->GetSpellInfoByTag(SelectedSpellTag).SpellAbilityClass)
		{
			if (const UAuraGameplayAbility* AuraAbilityCDO = SpellAbilityClass->GetDefaultObject<UAuraGameplayAbility>())
			{
				const AAuraPlayerState* AuraPS = GetAuraPlayerStateChecked();
				return AuraPS->GetCharacterLevel() >= AuraAbilityCDO->LevelRequirement;
			}
		}
	}

	return false;
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

void USpellMenuWidgetController::UpdateSpellPoints(int32 SpellPoints)
{
	bHasSpellPoints = SpellPoints > 0;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

