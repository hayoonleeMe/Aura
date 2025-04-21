// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameModeBase.h"
#include "Interface/StageSystemInterface.h"
#include "Types/StageStatus.h"
#include "StageGameMode.generated.h"

class UScalableStageConfig;
class AAuraGameStateBase;
struct FOnAttributeChangeData;
class UMultiplayerSessionsSubsystem;
class AAuraEnemy;

/**
 * Stage를 관리하는 Game Mode
 */
UCLASS()
class AURA_API AStageGameMode : public AAuraGameModeBase, public IStageSystemInterface
{
	GENERATED_BODY()

public:
	AStageGameMode();
	virtual void PostSeamlessTravel() override;

#if WITH_EDITOR
	// For Test in PIE, start at StartupMap
	virtual void PostLogin(APlayerController* NewPlayer) override;
#endif

	/* Begin IStageSystemInterface */
	virtual void OnStageBeaconInteracted() override;
	virtual void RequestPlayerRespawn(APlayerController* PlayerController) override;
	virtual void RequestSpawnEnemy(const TSubclassOf<AAuraEnemy>& EnemyClass, FTransform SpawnTransform, bool bOverrideLocationZ) override;
	virtual void OnPlayerASCInitialized(APlayerController* PC) override;
	/* End IStageSystemInterface */

	void WaitStageStart();
	void StartStage();
	void EndStage();

private:
	// ============================================================================
	// Stage
	// ============================================================================

	UPROPERTY(EditDefaultsOnly, Category="Aura|Stage")
	TObjectPtr<UScalableStageConfig> ScalableStageConfig;

	// 현재 상태
	EStageStatus StageStatus = EStageStatus::Waiting;

	// 현재 몇번 째 Stage인지 나타내는 수
	int32 StageNumber = 1;

	// 최대 진행할 수 있는 Stage 수
	int32 MaxStageNumber = INDEX_NONE;

	// 준비된 플레이어 수
	int32 ReadyPlayerCount = 0;

	void OnPlayerReady();

	UPROPERTY()
	TObjectPtr<AAuraGameStateBase> StageGameState;

	void SpawnStartStageBeacon() const;
	void DestroyStartStageBeacon() const;

	// 난이도 계산 시 사용할 최소 EnemySpawnLevel 기반 값 (너무 낮은 난이도를 방지하기 위한 하한선)
	UPROPERTY(EditDefaultsOnly, Category="Aura|Stage")
	float MinEnemySpawnLevelDifficulty;

	// 난이도 계산 시 EnemySpawnLevel에 곱해질 가중치
	UPROPERTY(EditDefaultsOnly, Category="Aura|Stage")
	float EnemySpawnLevelDifficultyWeight;

	// 스테이지 번호를 난이도에 적용할 때 사용하는 지수값
	UPROPERTY(EditDefaultsOnly, Category="Aura|Stage")
	float StageNumberDifficultyExponent;

	// 추가 플레이어 1명당 난이도 증가 비율
	UPROPERTY(EditDefaultsOnly, Category="Aura|Stage")
	float DifficultyIncreaseRatePerExtraPlayer;

	// EnemySpawnLevel에 따라 적 위험도를 스케일링하기 위한 비율
	UPROPERTY(EditDefaultsOnly, Category="Aura|Stage")
	float ThreatScalingPerEnemySpawnLevel;

	// ============================================================================
	// Game End
	// ============================================================================

	// 리스폰을 더 이상 할 수 없는 플레이어 수
	int32 RetiredPlayerCount = 0;

	// RetiredPlayerCount를 1 증가시키고 가능하면 EndGame() 수행
	void HandlePlayerRetire();

	// 게임 종료
	void EndGame() const;

	// 게임이 실제로 종료될 때까지의 딜레이
	UPROPERTY(EditDefaultsOnly,	Category="Aura|Game End")
	float GameEndDelaySeconds;

	// Destroy Session 작업이 완료됐을 때의 콜백 함수
	void OnDestroySessionComplete(bool bWasSuccessful) const;

	UPROPERTY()
	TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;

	// ============================================================================
	// Waiting Timer
	// ============================================================================

	// Stage Start를 기다리기 위한 Timer
	FTimerHandle WaitingTimerHandle;

	// WaitingTimer Callback Function and Delegate
	void OnWaitingTimeFinished();
	FTimerDelegate WaitingTimerDelegate;

	// Stage Start를 기다리는 시간
	UPROPERTY(EditDefaultsOnly, Category="Aura|Waiting Timer")
	float WaitingTime;

	void StartWaitingTimer();

	// ============================================================================
	// End Stage Delay Timer
	// ============================================================================

	// EndStage를 수행하기 전에 기다리는 시간
	UPROPERTY(EditDefaultsOnly, Category="Aura|End Stage Delay Timer")
	float EndStageDelay;

	// EndStageDelay를 기다리기 위한 Timer
	FTimerHandle EndStageDelayTimerHandle;

	// EndStageDelayTimerHandle Delegate, EndStage 연동
	FTimerDelegate EndStageDelayTimerDelegate;

	// ============================================================================
	// Spawn Enemy
	// ============================================================================

	// Enemy를 소환하는 Volume의 범위를 나타내는 Box
	FBox SpawnEnemyVolumeBox;
	
	// Enemy를 스폰할 때 사용할 파라미터
	FActorSpawnParameters EnemySpawnParams;

	// 스폰한 Enemy가 죽을 때를 알리는 델레게이트의 콜백 함수
	UFUNCTION()
	void OnEnemyDead();

	// Enemy를 현재 스테이지에 정해진 수만큼 모두 소환했는지
	bool bFinishSpawn = false;

	// 현재 Stage에 의해 소환된 Enemy 수
	int32 NumStageSpawnedEnemies = 0;

	// 죽은 Enemy 수
	// Shaman에 의해 소환된 Enemy도 포함
	int32 NumDeadEnemies = 0;

	// 전체 Enemy 수
	// Shaman에 의해 소환된 Enemy도 포함
	int32 TotalEnemyCount = 0;

	// Enemy 소환 간격 (초)
	// 최종 소환 간격 = SpawnWaitTime + FMath::FRandRange(-RandomDeviation, RandomDeviation)
	UPROPERTY(EditDefaultsOnly, Category="Aura|Spawn Enemy")
	float SpawnWaitTime;

	// SpawnWaitTime에 적용할 랜덤 편차
	// 최종 소환 간격 = SpawnWaitTime + FMath::FRandRange(-RandomDeviation, RandomDeviation)
	UPROPERTY(EditDefaultsOnly, Category="Aura|Spawn Enemy")
	float RandomDeviation;

	// 최종 소환 간격을 계산해 반환
	float GetRandomDelay() const { return SpawnWaitTime + FMath::FRandRange(-RandomDeviation, RandomDeviation); }

	// 한번에 소환할 수 있는 최대 Enemy 수
	UPROPERTY(EditDefaultsOnly, Category="Aura|Spawn Enemy")
	int32 MaxSpawnCountPerBatch;

	// 소환할 Enemy 레벨
	float EnemySpawnLevel = 1.f;

	// 플레이어의 Level Attribute 값이 변경될 때 호출되는 콜백 함수
	// GetGameplayAttributeValueChangeDelegate를 통해 해당 델리게이트에 바인딩됨
	void OnPlayerLevelAttributeChanged(const FOnAttributeChangeData& Data);

	// AAuraEnemy 타입 Enemy 소환
	void SpawnEnemy(const TSubclassOf<AAuraEnemy>& EnemyClass);

	// 비동기적으로 Enemy 소환
	// RandomDelay마다 랜덤한 수의 랜덤한 Enemy를 소환하고, 현재 스테이지에서 정해진 수만큼 모두 소환할 때까지 반복한다.
	void AsyncSpawnEnemies();
	FTimerHandle SpawnDelayTimerHandle;
	FTimerDelegate SpawnDelayTimerDelegate;

	// 소환할 Enemy Class
	UPROPERTY()
	TArray<TSubclassOf<AAuraEnemy>> EnemiesToSpawn;

	// 스테이지 난이도와 적의 위협 수치를 토대로 소환할 Enemy를 결정한다.
	void PrepareEnemySpawn();

	// ============================================================================
	// Respawn
	// ============================================================================

	// 플레이어의 최대 목숨 개수
	// Retrieved from AAuraGameStateBase
	int32 TotalLifeCount = 0;

	// 리스폰에 걸리는 시간
	// Retrieved from AAuraGameStateBase
	float RespawnTime = 0.f;

	// RespawnTimer Callback function 
	void OnRespawnTimerFinished(APlayerController* ControllerToRespawn);

	// ============================================================================
	// Helper
	// ============================================================================

	// 모든 로컬 플레이어의 UI에 StageStatus를 표시
	void BroadcastStageStatusChangeToAllLocalPlayers() const;

	void BroadcastTotalEnemyCountChangeToAllLocalPlayers() const;

	// 리스폰할 플레이어의 UI에 RespawnTimer를 표시
	void NotifyRespawnStartToLocalPlayer(APlayerController* ControllerToRespawn, bool bGameEnd) const;

	void BroadcastGameEndToAllLocalPlayers() const;

	// 모든 플레이어가 각 기기에서 유효한지 체크하기 위해 Polling
	void PollInit();
	FTimerHandle PollingTimerHandle;

	// PollInit()을 통한 적절한 시점에서 호출되어 초기화 수행
	void InitData();

	// Ability System Component가 초기화된 플레이어를 저장하는 Set
	// 플레이어의 ASC가 초기화되면 ServerRPC를 통해 추가된다.
	UPROPERTY()
	TSet<APlayerController*> ASCInitializedPlayers;
};
