// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/AuraInputConfig.h"

int32 UAuraInputConfig::GetInputIDForInputTag(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FAbilityInputMapping& Mapping : AbilityInputActions)
		{
			if (Mapping.InputTag.MatchesTagExact(InputTag))
			{
				return Mapping.InputID;
			}
		}
	}
	return INDEX_NONE;
}

FGameplayTag UAuraInputConfig::GetInputTagForInputID(int32 InputID)
{
	if (InputID != INDEX_NONE)
	{
		for (const FAbilityInputMapping& Mapping : AbilityInputActions)
		{
			if (Mapping.InputID == InputID)
			{
				return Mapping.InputTag;
			}
		}
	}
	return FGameplayTag::EmptyTag;
}

const UInputAction* UAuraInputConfig::GetInputActionForInputTag(const FGameplayTag& InputTag) const
{
	if (InputTag.IsValid())
	{
		for (const FAbilityInputMapping& Mapping : AbilityInputActions)
		{
			if (Mapping.InputTag.MatchesTagExact(InputTag))
			{
				return Mapping.InputAction;
			}
		}
	}
	return nullptr;
}
