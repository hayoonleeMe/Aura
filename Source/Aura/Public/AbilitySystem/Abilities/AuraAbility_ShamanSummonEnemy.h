// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraAbility_ShamanSummonEnemy.generated.h"

class AStageGameMode;
class AAuraEnemy;

/**
 * Delay마다 설정한 Enemy 중에서 랜덤한 Enemy를 소환하는 Shaman의 Ability
 */
UCLASS(HideCategories=("Aura|Input", "Aura|Spell"))
class AURA_API UAuraAbility_ShamanSummonEnemy : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UAuraAbility_ShamanSummonEnemy();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);

private:
	// Summon Anim Montage to play
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> SummonMontage;

	// 레벨에 따른 소환할 Enemy 수를 저장하는 Curve
	UPROPERTY(EditDefaultsOnly)
	FScalableFloat NumEnemiesCurve;

	// 현재 Ability 레벨에 따른 NumEnemies 값을 반환
	int32 GetNumEnemies() const;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AAuraEnemy>> EnemyClasses;

	const TSubclassOf<AAuraEnemy>& GetRandomEnemyClass() const;

	// 소환할 최대 Enemy 수
	int32 MaxSummonCount = 0;

	// 소환된 Enemy 수
	int32 NumSummonedEnemies = 0;

	UPROPERTY()
	TObjectPtr<AStageGameMode> StageGameMode;

	// 비동기적으로 Enemy 소환
	// RandomDelay마다 설정한 Enemy 중 랜덤한 Enemy를 하나 소환하고, MaxSummonCount만큼 소환할 때까지 반복한다.
	void AsyncSpawnEnemies();
	FTimerHandle SpawnDelayTimerHandle;
	FTimerDelegate SpawnDelayTimerDelegate;

	// Shaman 전방 기준 Enemy를 소환할 Cone 범위의 Half Angle
	UPROPERTY(EditDefaultsOnly)
	float SpawnConeHalfAngleDegrees;

	// Shaman에서 떨어진 SpawnLocation의 최소 거리
	UPROPERTY(EditDefaultsOnly)
	float SpawnMinDistance;

	// Shaman에서 떨어진 SpawnLocation의 최대 거리
	UPROPERTY(EditDefaultsOnly)
	float SpawnMaxDistance;

	// Enemy 소환 간격 (초)
	// 최종 소환 간격 = SpawnWaitTime + FMath::FRandRange(-RandomDeviation, RandomDeviation)
	UPROPERTY(EditDefaultsOnly)
	float SpawnWaitTime;

	// SpawnWaitTime에 적용할 랜덤 편차
	// 최종 소환 간격 = SpawnWaitTime + FMath::FRandRange(-RandomDeviation, RandomDeviation)
	UPROPERTY(EditDefaultsOnly)
	float RandomDeviation;

	// 최종 소환 간격을 계산해 반환
	float GetRandomDelay() const { return SpawnWaitTime + FMath::FRandRange(-RandomDeviation, RandomDeviation); }
};
