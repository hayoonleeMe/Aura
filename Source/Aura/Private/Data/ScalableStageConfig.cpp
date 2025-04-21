// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/ScalableStageConfig.h"

void UScalableStageConfig::PostLoad()
{
	Super::PostLoad();

	SortAscendingSpawnEnemyInfosByThreatValue();
}

void UScalableStageConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	SortAscendingSpawnEnemyInfosByThreatValue();
}

void UScalableStageConfig::SortAscendingSpawnEnemyInfosByThreatValue()
{
	SpawnEnemyInfos.Sort([](const FScalableSpawnEnemyInfo& A, const FScalableSpawnEnemyInfo& B)
	{
		return A.ThreatValue < B.ThreatValue;
	});
}
