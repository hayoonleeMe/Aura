// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/EnemyClassConfig.h"

const FEnemyClassInfo* UEnemyClassConfig::GetInfoByType(EEnemyClassType InType) const
{
	if (TypeToInfoMap.Contains(InType))
	{
		return &TypeToInfoMap[InType];
	}
	return nullptr;
}
