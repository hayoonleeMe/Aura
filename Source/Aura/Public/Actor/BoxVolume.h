// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxVolume.generated.h"

class UBoxComponent;

/**
 * 레벨에서 범위를 Box Extent로 지정하는 Actor
 */
UCLASS()
class AURA_API ABoxVolume : public AActor
{
	GENERATED_BODY()

public:
	ABoxVolume();
	
	// Volume의 Box가 차지하는 Bound 좌표를 Box로 반환한다.
	FBox GetVolumeBounds() const;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;
};
