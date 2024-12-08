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

	UFUNCTION(BlueprintCallable)
	static UCurveTable* GetDamageCalculationCoefficients(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	static void ApplyDamageEffect(const FDamageEffectParams& Params);

	// 데미지를 입히기 위해 서로 적인지를 판별하는 함수
	UFUNCTION(BlueprintCallable)
	static bool IsNotFriend(const AActor* Actor1, const AActor* Actor2);

	// CueLocation을 GameplayCueParameters로 전달하고 Replicate되는 GameplayCueNotify_Static을 실행
	UFUNCTION(BlueprintCallable)
	static void ExecuteGameplayCue(AActor* OwnerActor, const FGameplayTag& CueTag, const FVector& CueLocation);

	// 현재 월드에서 모든 살아 있는 PlayerState가 컨트롤하는 Pawn을 가져온다.
	UFUNCTION(BlueprintCallable)
	static void GetAlivePawnsFromPlayers(const UObject* WorldContextObject, TArray<AActor*>& OutPlayers);
};
