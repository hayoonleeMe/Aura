// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	// 클라이언트의 Spell Menu Widget에서 Spell을 Unlock한 뒤 Equip Button을 올바르게 활성화하기 위함
	OnActivatableAbilitiesReplicatedDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraGameplayAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraGameplayAbility->StartupInputTag);
			GiveAbility(AbilitySpec);

			// System Ability는 Give와 동시에 활성화
			if (AuraGameplayAbility->AbilityTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Abilities.System"))))
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	
	ABILITYLIST_SCOPE_LOCK()
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
			AbilitySpecInputPressed(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	
	ABILITYLIST_SCOPE_LOCK()
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	
	ABILITYLIST_SCOPE_LOCK()
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::ServerSpendPointButtonPressed_Implementation(const FGameplayTag& SpellTag, TSubclassOf<UGameplayAbility> SpellAbilityClass)
{
	// Spell Points가 있을 때, 1만큼 소모해 진행
	IPlayerInterface* PlayerInterface = CastChecked<IPlayerInterface>(GetAvatarActor());
	if (PlayerInterface->GetSpellPoints() <= 0)
	{
		return;
	}
	PlayerInterface->DecrementSpellPoints();
	
	if (IsSpellUnlocked(SpellTag))
	{
		UpgradeSpell(SpellTag);
	}
	else
	{
		checkf(SpellAbilityClass, TEXT("Need to set SpellAbilityClass for %s Tag"), *SpellTag.GetTagName().ToString());
		// 해당 Spell을 Unlock해 Give Ability
		UnlockSpell(SpellTag, SpellAbilityClass);
	}
}

bool UAuraAbilitySystemComponent::IsSpellUnlocked(const FGameplayTag& SpellTag)
{
	// Spell Ability가 Give되어 ActivatableAbilities에 존재하면 Unlock 된 것을 의미 
	return GetSpellSpecForSpellTag(SpellTag) != nullptr;
}

void UAuraAbilitySystemComponent::UnlockSpell(const FGameplayTag& SpellTag, const TSubclassOf<UGameplayAbility>& SpellAbilityClass)
{
	const FGameplayAbilitySpec SpellSpec(SpellAbilityClass);
	GiveAbility(SpellSpec);

	// Spell이 Unlock 됨을 전달해 UI에 표시
	ClientBroadcastSpellChange(SpellTag);
}

void UAuraAbilitySystemComponent::UpgradeSpell(const FGameplayTag& SpellTag)
{
	if (FGameplayAbilitySpec* SpellSpec = GetSpellSpecForSpellTag(SpellTag))
	{
		SpellSpec->Level += 1;
		MarkAbilitySpecDirty(*SpellSpec);

		// Spell의 Level 변경을 전달
		ClientBroadcastSpellChange(SpellTag);
	}
}

void UAuraAbilitySystemComponent::ServerHandleEquipSpell_Implementation(const FGameplayTag& SpellTagToEquip, const FGameplayTag& InputTag)
{
	FGameplayAbilitySpec* SpellSpecToEquip = GetSpellSpecForSpellTag(SpellTagToEquip);
	if (!SpellSpecToEquip)
	{
		return;
	}

	// Input에 이미 Spell이 장착되어 있다면
	if (FGameplayAbilitySpec* EquippedSpellSpec = GetSpellSpecForInputTag(InputTag))
	{
		// Input의 이미 장착된 Spell을 장착 해제
		UnEquipSpell(EquippedSpellSpec, InputTag, true);
		
		// 장착하려는 Spell이 Input에 이미 장착되어 있으므로 장착 해제만 수행하고 return
		if (EquippedSpellSpec == SpellSpecToEquip)
		{
			return;
		}
	}

	// 장착하고자 하는 Spell이 다른 Input에 장착되어 있다면, 그 Input에서 장착 해제
	const FGameplayTag PrevInputTag = GetInputTagForSpellSpec(SpellSpecToEquip);
	if (PrevInputTag.IsValid())
	{
		UnEquipSpell(SpellSpecToEquip, PrevInputTag, false);
	}
	
	// InputTag를 추가해 장착
	SpellSpecToEquip->DynamicAbilityTags.AddTag(InputTag);

	// InputTag에 Spell을 장착했음을 전달
	ClientBroadcastEquippedSpellChange(true, InputTag, SpellTagToEquip);

	MarkAbilitySpecDirty(*SpellSpecToEquip);

	// Passive Spell은 장착과 동시에 활성화
	if (SpellSpecToEquip->Ability->AbilityTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Abilities.Passive"))) && !SpellSpecToEquip->Ability->IsActive())
	{
		TryActivateAbility(SpellSpecToEquip->Handle);
	}
}

void UAuraAbilitySystemComponent::UnEquipSpell(FGameplayAbilitySpec* SpellSpecToUnEquip, const FGameplayTag& InputTagToRemove, bool bClearPassiveSpell)
{
	if (SpellSpecToUnEquip)
	{
		// InputTag 제거
		SpellSpecToUnEquip->DynamicAbilityTags.RemoveTag(InputTagToRemove);
		MarkAbilitySpecDirty(*SpellSpecToUnEquip);

		// InputTagToRemove이 나타내는 Input에 대한 UnEquip을 전달한다.
		ClientBroadcastEquippedSpellChange(false, InputTagToRemove, GetSpellTagForSpellSpec(SpellSpecToUnEquip));

		// Passive Spell은 장착과 동시에 실행되므로, 장착 해제 후 종료
		if (bClearPassiveSpell)
		{
			if (SpellSpecToUnEquip->Ability->AbilityTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Abilities.Passive"))))
			{
				CancelAbilitySpec(*SpellSpecToUnEquip, nullptr);
			}
		}
	}
}

void UAuraAbilitySystemComponent::ClientBroadcastSpellChange_Implementation(const FGameplayTag& SpellTag)
{
	OnSpellAbilityChangedDelegate.Broadcast(SpellTag);
}

void UAuraAbilitySystemComponent::ClientBroadcastEquippedSpellChange_Implementation(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag)
{
	OnEquippedSpellAbilityChangedDelegate.Broadcast(bEquipped, InputTag, SpellTag);
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpellSpecForSpellTag(const FGameplayTag& SpellTag)
{
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.Ability->AbilityTags.HasTagExact(SpellTag))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpellSpecForInputTag(const FGameplayTag& InputTag)
{
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagForSpellSpec(FGameplayAbilitySpec* SpellSpec)
{
	if (SpellSpec)
	{
		const FGameplayTag FilterTag = FGameplayTag::RequestGameplayTag(TEXT("InputTag"));
		for (const FGameplayTag& Tag : SpellSpec->DynamicAbilityTags)
		{
			if (Tag.MatchesTag(FilterTag))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag::EmptyTag;
}

FGameplayTag UAuraAbilitySystemComponent::GetSpellTagForSpellSpec(const FGameplayAbilitySpec* SpellSpec)
{
	if (SpellSpec)
	{
		const FGameplayTag OffensiveFilter = FGameplayTag::RequestGameplayTag(TEXT("Abilities.Offensive")); 
		const FGameplayTag PassiveFilter = FGameplayTag::RequestGameplayTag(TEXT("Abilities.Passive"));
		
		for (const FGameplayTag& Tag : SpellSpec->Ability->AbilityTags)
		{
			if (Tag.MatchesTag(OffensiveFilter) || Tag.MatchesTag(PassiveFilter))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag::EmptyTag;
}
