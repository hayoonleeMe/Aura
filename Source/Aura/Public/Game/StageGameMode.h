// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameModeBase.h"
#include "Types/StageStatus.h"
#include "StageGameMode.generated.h"

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
	
private:
	// ============================================================================
	// Stage
	// ============================================================================

	UPROPERTY()
	EStageStatus StageStatus;

	// 현재 몇번 째 Stage인지 나타내는 수
	UPROPERTY()
	int32 StageNumber;

	// ============================================================================
	// Waiting Timer
	// ============================================================================

	// Stage Start를 기다리기 위한 Timer
	FTimerHandle WaitingTimerHandle;

	// WaitingTimer Callback Function and Delegate
	void OnWaitingTimeFinished();
	FTimerDelegate WaitingTimerDelegate;

	// Stage Start를 기다리는 시간
	UPROPERTY(EditDefaultsOnly, Category="Waiting Timer")
	float WaitingTime;

	void StartWaitingTimer();

	// ============================================================================
	// Beacon
	// ============================================================================

	// Stage를 시작하는 Beacon Actor
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> StartStageBeaconClass;

	UPROPERTY()
	TObjectPtr<AActor> StartStageBeacon;

	void SpawnStartStageBeacon();

	// ============================================================================
	// Helper
	// ============================================================================

	// SimulatedProxy의 Auth Player Controller 반환
	APlayerController* GetSimulatedPlayerController() const;

	// 모든 로컬 플레이어의 UI에 StageStatus를 표시
	void BroadcastStageStatusChangeToAllLocalPlayers() const;

	// 모든 플레이어가 각 기기에서 유효한지 체크하기 위해 Polling
	void PollInit();
	FTimerHandle PollingTimerHandle;
};
