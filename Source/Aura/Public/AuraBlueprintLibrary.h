// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraBlueprintLibrary.generated.h"

class USpellMenuWidgetController;
class UOverlayWidgetController;
struct FWidgetControllerParams;
class UAttributeMenuWidgetController;
struct FGameplayTag;
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
	// ============================================================================
	// Gameplay
	// ============================================================================

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

	// 게임에서 기본으로 사용하는 Input Mode로 설정
	UFUNCTION(BlueprintCallable)
	static void SetInGameInputMode(const UObject* WorldContextObject);

	// UI 전용 Input Mode로 설정
	UFUNCTION(BlueprintCallable)
	static void SetUIInputMode(const UObject* WorldContextObject);

	// ============================================================================
	// UI
	// ============================================================================

	// OwningController의 HUD에 저장된 Overlay Widget Controller 반환
	UFUNCTION(BlueprintPure)
	static UOverlayWidgetController* GetOverlayWidgetController(const APlayerController* OwningController);
	
	// OwningController의 HUD에 저장된 Attribute Menu Widget Controller 반환
	UFUNCTION(BlueprintPure)
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const APlayerController* OwningController);

	// OwningController의 HUD에 저장된 Spell Menu Widget Controller 반환
	UFUNCTION(BlueprintPure)
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const APlayerController* OwningController);

	// ============================================================================
	// Math
	// ============================================================================

	// CentralDirection을 기준으로 SpreadAngle만큼의 간격으로 NumDirections개의 방향을 계산한다.
	UFUNCTION(BlueprintCallable)	
	static void GetSpreadDirections(TArray<FVector>& OutDirections, int32 NumDirections, float SpreadAngle, const FVector& CentralDirection);
};
