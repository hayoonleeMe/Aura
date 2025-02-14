// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StageConfig.generated.h"

class AAuraEnemy;

/**
 * 스테이지별 적 소환 정보
 */
USTRUCT(BlueprintType)
struct FSpawnEnemyInfo
{
	GENERATED_BODY()

	// Enemy class to spawn
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAuraEnemy> EnemyClass;
	
	// Number of enemies to spawn
	UPROPERTY(EditDefaultsOnly)
	int32 SpawnCount{0};
};

USTRUCT(BlueprintType)
struct FStageInfo
{
	GENERATED_BODY()

	// 스테이지별 적 소환 정보를 저장하는 배열
	UPROPERTY(EditDefaultsOnly)
	TArray<FSpawnEnemyInfo> SpawnEnemyInfos;
};

/**
 * Stage에 대한 정보를 저장하는 DataAsset
 */
UCLASS()
class AURA_API UStageConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	// StageNumber에 해당하는 FSpawnEnemyInfo를 반환한다.
	// 없으면 assert
	const TArray<FSpawnEnemyInfo>& GetSpawnEnemyInfosForStageNumberChecked(int32 StageNumber);
	
	int32 GetMaxStageNumber() const { return StageInfos.Num(); }
	
private:
	// 인덱스 + 1이 스테이지 번호를 의미
	UPROPERTY(EditDefaultsOnly)
	TArray<FStageInfo> StageInfos;
};
