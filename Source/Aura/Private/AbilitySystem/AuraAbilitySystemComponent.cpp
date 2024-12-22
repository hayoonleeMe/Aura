// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Player/AuraPlayerState.h"

void UAuraAbilitySystemComponent::AddAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraGameplayAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraGameplayAbility->InputTag);
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
	// TODO : 새 Interface를 사용해 AAuraPlayerState에 의존하지 않고 Spell Point에 접근하도록 변경
	if (AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(GetOwnerActor()))
	{
		if (AuraPS->GetSpellPoints() <= 0)
		{
			return;
		}
		AuraPS->DecrementSpellPoints();
	}
	
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
	if (const FGameplayAbilitySpec* SpellSpec = GetSpellSpecForSpellTag(SpellTag))
	{
		return SpellSpec->DynamicAbilityTags.HasTagExact(FAuraGameplayTags::Get().SpellStatus_Unlocked) || SpellSpec->DynamicAbilityTags.HasTagExact(FAuraGameplayTags::Get().SpellStatus_Equipped);
	}
	return false;
}

void UAuraAbilitySystemComponent::UnlockSpell(const FGameplayTag& SpellTag, const TSubclassOf<UGameplayAbility>& SpellAbilityClass)
{
	FGameplayAbilitySpec SpellSpec(SpellAbilityClass, 1);
	SpellSpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().SpellStatus_Unlocked);
	GiveAbility(SpellSpec);

	// Spell이 Unlock 됨을 전달해 UI에 표시
	ClientBroadcastSpellChange(SpellTag, SpellSpec.Level);
}

void UAuraAbilitySystemComponent::UpgradeSpell(const FGameplayTag& SpellTag)
{
	if (FGameplayAbilitySpec* SpellSpec = GetSpellSpecForSpellTag(SpellTag))
	{
		SpellSpec->Level += 1;	// TODO : Need to Clamp
		MarkAbilitySpecDirty(*SpellSpec);

		// Spell의 Level 변경을 전달
		ClientBroadcastSpellChange(SpellTag, SpellSpec->Level);
	}
}

void UAuraAbilitySystemComponent::ClientBroadcastSpellChange_Implementation(const FGameplayTag& SpellTag, int32 SpellLevel)
{
	OnSpellAbilityChangedDelegate.Broadcast(SpellTag, SpellLevel);
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

