// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameModeBase.h"
#include "Types/StageStatus.h"
#include "StageGameMode.generated.h"

class UMultiplayerSessionsSubsystem;
class AAuraEnemy;
class UStageConfig;

/**
 * Stage를 관리하는 Game Mode
 */
UCLASS()
class AURA_API AStageGameMode : public AAuraGameModeBase
{
	GENERATED_BODY()

public:
	AStageGameMode();
	virtual void PostSeamlessTravel() override;

#if WITH_EDITOR
	// For Test in PIE, start at StartupMap
	virtual void PostLogin(APlayerController* NewPlayer) override;
#endif

	void WaitStageStart();
	void StartStage();
	void EndStage();

	// 가능하면 리스폰 수행
	void RequestPlayerRespawn(APlayerController* PlayerController);
	
private:
	// ============================================================================
	// Stage
	// ============================================================================

	UPROPERTY(EditDefaultsOnly, Category="Aura|Stage")
	TObjectPtr<UStageConfig> StageConfig;

	// 현재 상태
	EStageStatus StageStatus = EStageStatus::Waiting;

	// 현재 몇번 째 Stage인지 나타내는 수
	int32 StageNumber = 1;

	// 최대 진행할 수 있는 Stage 수
	int32 MaxStageNumber = INDEX_NONE;

	// ============================================================================
	// Game End
	// ============================================================================

	// 리스폰을 더 이상 할 수 없는 플레이어 수
	int32 RetiredPlayerCount = 0;

	// RetiredPlayerCount를 1 증가시키고 가능하면 EndGame() 수행
	void HandlePlayerRetire();

	// 게임 종료
	void EndGame();

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
	// Beacon
	// ============================================================================

	// Stage를 시작하는 Beacon Actor
	UPROPERTY(EditDefaultsOnly, Category="Aura|Beacon")
	TSubclassOf<AActor> StartStageBeaconClass;

	UPROPERTY()
	TObjectPtr<AActor> StartStageBeacon;

	void SpawnStartStageBeacon();

	// ============================================================================
	// Spawn Enemy
	// ============================================================================

	// Enemy를 소환하는 Volume의 범위를 나타내는 Box
	FBox SpawnEnemyVolumeBox;
	
	// 이 게임 모드에서 액터를 스폰하는 데 사용할 파라미터
	FActorSpawnParameters SpawnParams;

	// 스폰한 Enemy가 죽을 때를 알리는 델레게이트의 콜백 함수
	UFUNCTION()
	void OnEnemyDead();

	// Enemy를 현재 스테이지에 정해진 수만큼 모두 소환했는지
	bool bFinishSpawn = false;

	// 현재 소환된 Enemy 수
	int32 NumSpawnedEnemies = 0;

	// 죽은 Enemy 수
	int32 NumDeadEnemies = 0;

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
	int32 MaxSpawnCount;

	// AAuraEnemy 타입 Enemy 소환
	void SpawnEnemy(TSubclassOf<AAuraEnemy> Class);

	// 비동기적으로 Enemy 소환
	// RandomDelay마다 랜덤한 수의 랜덤한 Enemy를 소환하고, 현재 스테이지에서 정해진 수만큼 모두 소환할 때까지 반복한다.
	void AsyncSpawnEnemies();
	FTimerHandle SpawnDelayTimerHandle;
	FTimerDelegate SpawnDelayTimerDelegate;

	// Enemy를 소환할 때 종류에 상관없이 랜덤하게 소환할 수 있도록 설정한다.
	// 현재 스테이지에 정해진 모든 Enemy 정보를 하나의 배열에 저장하고, 이를 Shuffle하여 랜덤한 수만큼 소환하는 방식을 사용한다.
	void PrepareEnemySpawn();
	
	// EnemyClass를 나타내는 uint8 값을 저장하는 배열
	TArray<uint8> RandomEnemyInfos;
	
	// uint8 <-> EnemyClass 간의 변환 매핑을 저장하는 맵
	TMap<uint8, TSubclassOf<AAuraEnemy>> EnemyClassTable;

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

	// SimulatedProxy의 Auth Player Controller 반환
	APlayerController* GetSimulatedPlayerController() const;

	// 모든 로컬 플레이어의 UI에 StageStatus를 표시
	void BroadcastStageStatusChangeToAllLocalPlayers() const;

	// 리스폰할 플레이어의 UI에 RespawnTimer를 표시
	void NotifyRespawnStartToLocalPlayer(APlayerController* ControllerToRespawn, bool bGameEnd) const;

	// 모든 플레이어가 각 기기에서 유효한지 체크하기 위해 Polling
	void PollInit();
	FTimerHandle PollingTimerHandle;

	// PollInit()을 통한 적절한 시점에서 호출되어 초기화 수행
	void InitData();
};
