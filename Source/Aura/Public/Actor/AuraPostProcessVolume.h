// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/PostProcessVolume.h"
#include "AuraPostProcessVolume.generated.h"

UCLASS()
class AURA_API AAuraPostProcessVolume : public APostProcessVolume
{
	GENERATED_BODY()

public:
	AAuraPostProcessVolume();

	// 화면 밝기 적용
	void ApplyBrightness(float BrightnessValue);

protected:
	virtual void BeginPlay() override;
};
