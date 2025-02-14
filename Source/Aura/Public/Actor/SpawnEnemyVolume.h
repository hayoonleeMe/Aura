// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnEnemyVolume.generated.h"

class UBoxComponent;

/**
 * Enemy를 소환할 범위를 Box Extent로 지정하는 Actor
 */
UCLASS()
class AURA_API ASpawnEnemyVolume : public AActor
{
	GENERATED_BODY()

public:
	ASpawnEnemyVolume();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;
};
