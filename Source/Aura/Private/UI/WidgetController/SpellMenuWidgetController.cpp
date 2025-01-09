// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Data/SpellConfig.h"
#include "Player/AuraPlayerState.h"

USpellMenuWidgetController::USpellMenuWidgetController()
{
	bWaitSelectGlobe = false;
	bHasSpellPoints = false;
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	UpdateStartupSpells();
	
	const AAuraPlayerState* AuraPS = GetAuraPlayerStateChecked();
	UpdateSpellPoints(AuraPS->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAbilitySystemComponent* AuraASC = GetAuraAbilitySystemComponentChecked();
	
	// Spell의 변경을 전달
	AuraASC->OnSpellAbilityChangedDelegate.AddUObject(this, &ThisClass::UpdateSpellChange);

	// Spell의 장착 상황 변경을 전달
	AuraASC->OnEquippedSpellAbilityChangedDelegate.AddUObject(this, &ThisClass::UpdateEquippedSpellChange);

	// 클라이언트에서 Spell을 Unlock할 때 Equip Button을 올바르게 활성화하기 위해 바인딩
	AuraASC->OnActivatableAbilitiesReplicatedDelegate.AddUObject(this, &ThisClass::OnSpellGiven);
	
	// SpellPoints가 변경되면 그 값을 전달
	AAuraPlayerState* AuraPS = GetAuraPlayerStateChecked();
	AuraPS->OnSpellPointsChangedDelegate.AddUObject(this, &ThisClass::UpdateSpellPoints);
}

bool USpellMenuWidgetController::IsSelectedSpellOffensive() const
{
	return SelectedSpellTag.MatchesTag(FAuraGameplayTags::Get().Abilities_Offensive);
}

bool USpellMenuWidgetController::CanEquipSpell() const
{
	UAuraAbilitySystemComponent* AuraASC = GetAuraAbilitySystemComponentChecked();
	return SelectedSpellTag.IsValid() && AuraASC->IsSpellUnlocked(SelectedSpellTag);
}

bool USpellMenuWidgetController::CanSpendPoint() const
{
	// Spell Point가 없거나 선택한 Spell Globe가 없으므로 불가능
	if (!bHasSpellPoints || !SelectedSpellTag.IsValid())
	{
		return false;
	}

	UAuraAbilitySystemComponent* AuraASC = GetAuraAbilitySystemComponentChecked();
	if (const FGameplayAbilitySpec* SpellSpec = AuraASC->GetSpellSpecForSpellTag(SelectedSpellTag))
	{
		// Spell Spec을 가져올 수 있으면 이미 Unlock 된 것을 의미
		// Unlock 된 상태라면, Spell을 더 레벨업 할 수 있는지 반환
		const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(SpellSpec->Ability);
		return AuraAbility && SpellSpec->Level < AuraAbility->MaxSpellLevel; 
	}

	if (SpellConfig)
	{
		if (const TSubclassOf<UGameplayAbility>& SpellAbilityClass = SpellConfig->GetSpellInfoByTag(SelectedSpellTag).SpellAbilityClass)
		{
			if (const UAuraGameplayAbility* AuraAbilityCDO = SpellAbilityClass->GetDefaultObject<UAuraGameplayAbility>())
			{
				// 현재 플레이어의 레벨이 Spell을 Unlock 할 수 있는 레벨인지 반환
				const int32 PlayerLevel = GetAuraAttributeSetChecked()->GetLevel();
				return PlayerLevel >= AuraAbilityCDO->LevelRequirement;
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

		// Spell Globe가 선택 해제될 때 Description 업데이트
		UpdateDescription(false);
	}
	else
	{
		// 기존 Spell Globe 선택 해제
		SelectSpellGlobeDelegate.Broadcast(false, SelectedSpellTag, false);
		
		// 새 Spell Globe 선택
		SelectedSpellTag = SpellTag;
		SelectSpellGlobeDelegate.Broadcast(true, SelectedSpellTag, false);

		// Spell Globe를 선택할 때 Description 업데이트
		UpdateDescription(true);
	}
}

void USpellMenuWidgetController::DeselectSpellGlobe()
{
	if (SelectedSpellTag.IsValid())
	{
		const FGameplayTag DupSelectedSpellTag = SelectedSpellTag;
		SelectedSpellTag = FGameplayTag::EmptyTag;
		SelectSpellGlobeDelegate.Broadcast(false, DupSelectedSpellTag, false);

		// Spell Globe가 선택 해제될 때 Description 업데이트
		UpdateDescription(false);
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

void USpellMenuWidgetController::UpdateSpellChange(const FGameplayTag& SpellTag) const
{
	if (SpellConfig)
	{
		OnSpellChangedDelegate.Broadcast(SpellConfig->GetSpellInfoByTag(SpellTag));

		UpdateDescription(true);
	}
}

void USpellMenuWidgetController::UpdateEquippedSpellChange(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag) const
{
	if (SpellConfig)
	{
		const FSpellInfo SpellInfo = SpellConfig->GetSpellInfoByTag(SpellTag);
		OnEquippedSpellChangedDelegate.Broadcast(bEquipped, InputTag, SpellInfo);
	}
}

void USpellMenuWidgetController::OnSpellGiven() const
{
	// Spell Menu에 전달
	OnSpellGivenDelegate.Broadcast();
	UpdateDescription(true);
}

void USpellMenuWidgetController::UpdateStartupSpells() const
{
	UAuraAbilitySystemComponent* AuraASC = GetAuraAbilitySystemComponentChecked();
	
	TArray<TTuple<FGameplayTag, FGameplayTag>> StartupSpells;
	AuraASC->GetSpellAndInputTagPairs(StartupSpells);
	for (const TTuple<FGameplayTag, FGameplayTag>& Tuple : StartupSpells)
	{
		UpdateSpellChange(Tuple.Key);
		UpdateEquippedSpellChange(true, Tuple.Value, Tuple.Key);
	}
}

void USpellMenuWidgetController::UpdateDescription(bool bSelected) const
{
	if (bSelected)
	{
		// 현재 Spell Ability Level에 맞는 Description 전달
		UAuraAbilitySystemComponent* AuraASC = GetAuraAbilitySystemComponentChecked();
		if (const FGameplayAbilitySpec* SpellSpec = AuraASC->GetSpellSpecForSpellTag(SelectedSpellTag))
		{
			// SpellSpec을 구할 수 있다는 것은 해당 Spell이 이미 Unlock 됐다는 것을 의미
			if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(SpellSpec->Ability))
			{
				// Spell이 최대 레벨이면 Next Level Description은 표시 X
				const FText NextLevelDescriptionText = SpellSpec->Level < AuraAbility->MaxSpellLevel ? AuraAbility->GetDescription(SpellSpec->Level + 1) : FText();
				const FText DescriptionText = AuraAbility->GetDescription(SpellSpec->Level);
				OnDescriptionUpdatedDelegate.Broadcast(DescriptionText, NextLevelDescriptionText);
			}
		}
		else
		{
			if (SpellConfig)
			{
				if (const TSubclassOf<UGameplayAbility>& SpellAbilityClass = SpellConfig->GetSpellInfoByTag(SelectedSpellTag).SpellAbilityClass)
				{
					if (const UAuraGameplayAbility* AuraAbilityCDO = Cast<UAuraGameplayAbility>(SpellAbilityClass.GetDefaultObject()))
					{
						// Locked Description
						OnDescriptionUpdatedDelegate.Broadcast(AuraAbilityCDO->GetLockedDescription(), FText());
					}
				}
			}
		}
	}
	else
	{
		// Description Widget Text 제거
		OnDescriptionUpdatedDelegate.Broadcast(FText(), FText());		
	}
}
