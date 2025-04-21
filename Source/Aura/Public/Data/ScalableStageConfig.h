// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ScalableStageConfig.generated.h"

class AAuraEnemy;

USTRUCT(BlueprintType)
struct FScalableSpawnEnemyInfo
{
	GENERATED_BODY()

	FScalableSpawnEnemyInfo()
		: ThreatValue(0.f)
	{}

	// 소환할 적 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAuraEnemy> EnemyClass;

	// 위험도
	UPROPERTY(EditDefaultsOnly)
	float ThreatValue;
};

/**
 * 
 */
UCLASS()
class AURA_API UScalableStageConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void PostLoad() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	UPROPERTY(EditDefaultsOnly)
	int32 MaxStage;

	// 항상 ThreatValue를 기준으로 오름차순 정렬된다.
	UPROPERTY(EditDefaultsOnly)
	TArray<FScalableSpawnEnemyInfo> SpawnEnemyInfos;

private:
	void SortAscendingSpawnEnemyInfosByThreatValue();
};
