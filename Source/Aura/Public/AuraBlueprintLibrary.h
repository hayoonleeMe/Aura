// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/CombatInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraBlueprintLibrary.generated.h"

class ALevelSequenceActor;
struct FGameplayAbilityTargetDataHandle;
struct FGameplayCueParameters;
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

	// Level에 해당하는 ArmorPenetrationCoefficient 값을 반환
	UFUNCTION(BlueprintCallable)
	static float GetArmorPenetrationCoefficientByLevel(const UObject* WorldContextObject, float Level);

	// Level에 해당하는 EffectiveArmorCoefficient 값을 반환
	UFUNCTION(BlueprintCallable)
	static float GetEffectiveArmorCoefficientByLevel(const UObject* WorldContextObject, float Level);

	// Level에 해당하는 CriticalHitResistanceCoefficient 값을 반환
	UFUNCTION(BlueprintCallable)
	static float GetCriticalHitResistanceCoefficientByLevel(const UObject* WorldContextObject, float Level);

	// TargetAbilitySystemComponent에 Damage Effect를 적용한다.
	// 적용 성공 여부를 반환한다.
	UFUNCTION(BlueprintCallable)
	static bool ApplyDamageEffect(const FDamageEffectParams& Params);

	// 데미지를 입히기 위해 서로 적인지를 판별하는 함수
	UFUNCTION(BlueprintCallable)
	static bool IsNotFriend(const AActor* Actor1, const AActor* Actor2);

	// CueLocation을 GameplayCueParameters로 전달하고 Replicate되는 GameplayCueNotify_Static을 실행
	UFUNCTION(BlueprintCallable)
	static void ExecuteGameplayCue(AActor* OwnerActor, const FGameplayTag& CueTag, const FVector& CueLocation);

	UFUNCTION(BlueprintCallable)
	static void ExecuteGameplayCueWithParams(AActor* OwnerActor, const FGameplayTag& CueTag, const FGameplayCueParameters& CueParameters);

	// Tag에 해당하는 TaggedCombatInfo의 CombatSocketName을 이용해 OwnerActor의 CombatSocketLocation을 반환
	UFUNCTION(BlueprintCallable)
	static FVector GetCombatSocketLocationForTag(AActor* OwnerActor, FGameplayTag Tag);

	// 현재 월드에서 모든 살아 있는 PlayerState가 컨트롤하는 Pawn을 가져온다.
	UFUNCTION(BlueprintCallable)
	static void GetAlivePawnsFromPlayers(const UObject* WorldContextObject, TArray<AActor*>& OutPlayers);

	// TraceChannel에 Overlap된 Enemy를 반환한다.
	static void GetEnemiesOverlappedByChannel(const UWorld* World, TArray<AActor*>& OutEnemies, const FVector& Pos, const FQuat& Rot, ECollisionChannel TraceChannel, const FCollisionShape& CollisionShape);

	// Actor의 최하단 위치를 반환한다.
	// 구할 수 없다면 ZeroVector를 반환한다.
	UFUNCTION(BlueprintCallable)
	static FVector GetActorFeetLocation(const AActor* Actor);

	// 게임에서 기본으로 사용하는 Input Mode로 설정
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void SetInGameInputMode(const UObject* WorldContextObject);

	// UI 전용 Input Mode로 설정
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void SetUIInputMode(const UObject* WorldContextObject);

	// Actor의 Level Attribute 값을 반환
	UFUNCTION(BlueprintCallable)
	static int32 GetLevelAttributeValue(AActor* Actor);

	// Level에 도달하는 데 필요한 XP 값을 반환
	UFUNCTION(BlueprintPure)
	static int32 GetLevelUpXPRequirement(const APlayerController* OwningController, int32 Level);

	// Returns the Cursor Target for a given index if it exists
	static TWeakObjectPtr<AActor> GetCursorTargetFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index);

	// World에서 Tag를 가지는 Level Sequence Actor를 찾아 반환한다.
	// 존재하지 않으면 nullptr를 반환한다.
	static ALevelSequenceActor* GetLevelSequenceActorMatchingTag(const UWorld* World, const FName& Tag);

	// AAuraGameStateBase에 캐싱된 PauseMenu Level Sequence Actor를 반환한다.
	// 존재하지 않으면 nullptr를 반환한다.
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static ALevelSequenceActor* GetPauseMenuLevelSequenceActor(const UObject* WorldContextObject);

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
