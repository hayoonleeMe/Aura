// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/StageConfig.h"

const TArray<FSpawnEnemyInfo>& UStageConfig::GetSpawnEnemyInfosForStageNumberChecked(int32 StageNumber)
{
	check(StageInfos.IsValidIndex(StageNumber - 1));
	return StageInfos[StageNumber - 1].SpawnEnemyInfos;
}
