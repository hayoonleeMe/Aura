// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "Interface/CombatInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraBlueprintLibrary.generated.h"

class UAbilitySystemComponent;
class UAuraAbilitySystemComponent;
class AAuraGameStateBase;
class UAuraAttributeSet;
class ALevelSequenceActor;
struct FGameplayAbilityTargetDataHandle;
struct FGameplayCueParameters;
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

	// Strength Value에 해당하는 StrengthDamageCoefficient 값을 반환
	UFUNCTION(BlueprintCallable)
	static float GetStrengthDamageCoefficientByValue(const UObject* WorldContextObject, float Value);

	// Intelligence Value에 해당하는 IntelligenceDamageCoefficient 값을 반환
	UFUNCTION(BlueprintCallable)
	static float GetIntelligenceDamageCoefficientByValue(const UObject* WorldContextObject, float Value);

	// TargetAbilitySystemComponent에 Damage Effect를 적용한다.
	// 적용 성공 여부를 반환한다.
	UFUNCTION(BlueprintCallable)
	static bool ApplyDamageEffect(const FDamageEffectParams& Params);

	// 데미지를 입히기 위해 서로 적인지를 판별하는 함수
	UFUNCTION(BlueprintCallable)
	static bool IsNotFriend(const AActor* Actor1, const AActor* Actor2);

	// CueLocation을 GameplayCueParameters로 전달하고 Replicate되는 GameplayCueNotify_Static을 실행
	UFUNCTION(BlueprintCallable)
	static void ExecuteGameplayCue(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& CueTag, const FVector& CueLocation);

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

	// Actor의 Level Attribute 값을 반환
	UFUNCTION(BlueprintCallable)
	static int32 GetLevelAttributeValue(AActor* Actor);

	// Level에 도달하는 데 필요한 XP 값을 반환
	UFUNCTION(BlueprintPure)
	static int32 GetLevelUpXPRequirement(const APlayerController* OwningController, int32 Level);

	// Returns the Cursor Target for a given index if it exists
	static TWeakObjectPtr<AActor> GetCursorTargetFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index);

	// World에서 Tag를 가지는 T 타입 액터를 찾아 반환한다.
	// 존재하지 않으면 nullptr를 반환한다.
	template <class T>
	static T* GetActorInWorldForTag(const UWorld* World, const FName& Tag)
	{
		for (TActorIterator<T> It(World); It; ++It)
		{
			if (It && It->ActorHasTag(Tag))
			{
				return *It;
			}
		}
		return nullptr;
	}

	// AGameStateBase::GetServerWorldTimeSeconds()를 float 형으로 반환한다.
	// 구할 수 없으면 0.f을 반환한다.
	static float GetServerWorldTimeSecondsAsFloat(const UWorld* World);

	static UAuraAbilitySystemComponent* GetAuraAbilitySystemComponentChecked(APlayerState* PlayerState);
	static UAuraAttributeSet* GetAuraAttributeSetChecked(APlayerState* PlayerState);

	// SimulatedProxy의 Auth Player Controller 반환
	static APlayerController* GetSimulatedPlayerController(const UWorld* World);

	// ============================================================================
	// Math
	// ============================================================================

	// CentralDirection을 기준으로 SpreadAngle만큼의 간격으로 NumDirections개의 방향을 계산한다.
	UFUNCTION(BlueprintCallable)	
	static void GetSpreadDirections(TArray<FVector>& OutDirections, int32 NumDirections, float SpreadAngle, const FVector& CentralDirection);
};
