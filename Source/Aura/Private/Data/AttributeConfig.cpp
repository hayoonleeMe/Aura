// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/AttributeConfig.h"

FAttributeInfo UAttributeConfig::GetAttributeInfoByTag(const FGameplayTag& Tag) const
{
	for (const FAttributeInfo& AttributeInfo : AttributeInfos)
	{
		if (AttributeInfo.AttributeTag.MatchesTagExact(Tag))
		{
			return AttributeInfo;
		}
	}
	return FAttributeInfo();
}
