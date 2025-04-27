// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Interface/ObjectPoolInterface.h"
#include "AuraGameStateBase.generated.h"

class UObjectPoolComponent;
class ABeacon_StartStage;

/**
 * 
 */
UCLASS()
class AURA_API AAuraGameStateBase : public AGameStateBase, public IObjectPoolInterface
{
	GENERATED_BODY()

public:
	AAuraGameStateBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Begin ObjectPoolInterface */
	virtual AActor* SpawnFromPool(EPooledActorType PooledActorType, const FTransform& SpawnTransform, bool bSetInUse) override;
	/* End ObjectPoolInterface */

	void SpawnStartStageBeacon();
	void DestroyStartStageBeacon() const;

	int32 GetTotalLifeCount() const { return TotalLifeCount; }
	float GetRespawnTime() const { return RespawnTime; }

	bool IsStartStageBeaconValid() const;

	// Stage Status changed to Started
	void OnStageStarted() const;

protected:
	virtual void BeginPlay() override;

private:
	// ============================================================================
	// Respawn
	// ============================================================================

	// 플레이어의 최대 목숨 개수
	UPROPERTY(EditDefaultsOnly, Category="Aura|Respawn")
	int32 TotalLifeCount;

	// 리스폰에 걸리는 시간
	UPROPERTY(EditDefaultsOnly, Category="Aura|Respawn")
	float RespawnTime;

	// ============================================================================
	// Beacon
	// ============================================================================

	// Stage를 시작하는 Beacon Actor
	UPROPERTY(EditDefaultsOnly, Category="Aura|Beacon")
	TSubclassOf<ABeacon_StartStage> StartStageBeaconClass;

	UPROPERTY(ReplicatedUsing = OnRep_StartStageBeacon)
	TObjectPtr<ABeacon_StartStage> StartStageBeacon;

	UFUNCTION()
	void OnRep_StartStageBeacon();

	// 플레이어로부터 Beacon을 스폰할 거리
	UPROPERTY(EditDefaultsOnly, Category="Aura|Beacon")
	float BeaconSpawnDistance;

	void PlaySpawnBeaconLevelSequence();

	void OnLevelSequenceStop(const FName& LevelSequenceTag);

	// 첫번째 Spawn Beacon Level Sequence가 끝나기를 기다리는 중
	// true라면, 첫 Spawn Beacon Level Sequence가 끝날 때 초기 로직 처리
	bool bWaitingForFirstLevelSequenceStop = true;

	// ============================================================================
	// Pool 
	// ============================================================================

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UObjectPoolComponent> FireBoltPoolComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UObjectPoolComponent> FireBallPoolComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UObjectPoolComponent> EmberBoltPoolComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UObjectPoolComponent> RockPoolComponent;
};
