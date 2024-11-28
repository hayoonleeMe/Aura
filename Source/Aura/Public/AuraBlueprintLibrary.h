// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraBlueprintLibrary.generated.h"

struct FDamageEffectParams;
class UEnemyClassConfig;

/**
 * C++과 Blueprint에서 사용할 수 있는 Helper Function Library
 */
UCLASS()
class AURA_API UAuraBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static UEnemyClassConfig* GetEnemyClassConfig(const UObject* WorldContextObject);
};
