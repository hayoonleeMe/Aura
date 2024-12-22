// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/SpellConfig.h"

#include "GameplayTagContainer.h"

FSpellInfo USpellConfig::GetSpellInfoByTag(const FGameplayTag& Tag) const
{
	for (const FSpellInfo& SpellInfo : SpellInfos)
	{
		if (SpellInfo.SpellTag.MatchesTagExact(Tag))
		{
			return SpellInfo;
		}
	}
	return FSpellInfo();
}
