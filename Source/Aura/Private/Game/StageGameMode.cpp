// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/StageGameMode.h"

#include "Player/AuraPlayerController.h"

// For Test in PIE
#if WITH_EDITOR
#include "Settings/LevelEditorPlaySettings.h"
#endif

AStageGameMode::AStageGameMode()
{
	WaitingTime = 5.f;	// TODO : Need to set proper value
	/* Waiting Timer */
	WaitingTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::OnWaitingTimeFinished);
}

#if WITH_EDITOR
void AStageGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// PIE로 테스트 시 모든 플레이어가 서버에 접속할 때
	const ULevelEditorPlaySettings* PlayInSettings = GetDefault<ULevelEditorPlaySettings>();
	int32 PlayNumberOfClients = 0;
	PlayInSettings->GetPlayNumberOfClients(PlayNumberOfClients);

	if (GetWorld()->GetNumPlayerControllers() == PlayNumberOfClients)
	{
		GetWorldTimerManager().SetTimer(PollingTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::PollInit), 0.1f, true);
	}
}
#endif

// SeamlessTravel - Lobby to StartupMap
void AStageGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	// Polling으로 모든 플레이어가 각 기기에서 유효한 상태일 때 WaitStageStart 호출
	GetWorldTimerManager().SetTimer(PollingTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::PollInit), 0.1f, true);
}

void AStageGameMode::WaitStageStart()
{
	BroadcastStageStatusChangeToAllLocalPlayers();
	SpawnStartStageBeacon();
	StartWaitingTimer();
}

void AStageGameMode::StartStage()
{
	StageStatus = EStageStatus::Started;

	GetWorldTimerManager().ClearTimer(WaitingTimerHandle);
	BroadcastStageStatusChangeToAllLocalPlayers();
	// TODO : Spawn Enemy
}

void AStageGameMode::EndStage()
{
	// 특정 조건을 만족해 호출됨 (모든 적 제거)

	StageStatus = EStageStatus::Waiting;
	StageNumber++;

	WaitStageStart();
}

void AStageGameMode::PollInit()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (!It->IsValid() || !IsValid(It->Get()->AcknowledgedPawn))
		{
			return;
		}
	}
	GetWorldTimerManager().ClearTimer(PollingTimerHandle);
	WaitStageStart();
}

void AStageGameMode::StartWaitingTimer()
{
	GetWorldTimerManager().SetTimer(WaitingTimerHandle, WaitingTimerDelegate, WaitingTime, false);
}

void AStageGameMode::OnWaitingTimeFinished()
{
	if (IsValid(StartStageBeacon))
	{
		StartStageBeacon->Destroy();
	}
	StartStage();
}

void AStageGameMode::SpawnStartStageBeacon()
{
	if (IsValid(StartStageBeacon))
	{
		return;
	}

	check(StartStageBeaconClass);

	// Beacon의 Ownership 설정을 위해 SimulatedProxy가 사용하는 PlayerController를 Owner로 설정 
	FActorSpawnParameters Params;
	Params.Owner = GetSimulatedPlayerController();
	StartStageBeacon = GetWorld()->SpawnActor<AActor>(StartStageBeaconClass, FTransform::Identity, Params);
}

APlayerController* AStageGameMode::GetSimulatedPlayerController() const
{
	if (GetWorld())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (It->IsValid() && !It->Get()->IsLocalController())
			{
				return It->Get();
			}
		}
	}
	return nullptr;
}

void AStageGameMode::BroadcastStageStatusChangeToAllLocalPlayers() const
{
	if (GetWorld())
	{
		const double WaitingTimerEndSeconds = StageStatus == EStageStatus::Waiting ? GetWorld()->GetTimeSeconds() + WaitingTime : 0.0;

		// 모든 Player Controller의 Multicast RPC를 호출해 로컬 플레이어의 화면에 표시
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(It->Get()))
			{
				AuraPC->MulticastOnStageStatusChanged(StageStatus, StageNumber, WaitingTimerEndSeconds);
			}
		}
	}
}
