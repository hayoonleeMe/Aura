// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/StageGameMode.h"

#include "Actor/SpawnEnemyVolume.h"
#include "Algo/RandomShuffle.h"
#include "Character/AuraEnemy.h"
#include "Components/BoxComponent.h"
#include "Data/StageConfig.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/AuraPlayerController.h"

// For Test in PIE
#if WITH_EDITOR
#include "Settings/LevelEditorPlaySettings.h"
#endif

AStageGameMode::AStageGameMode()
{
	/* Waiting Timer */
	WaitingTime = 10.f;
	WaitingTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::OnWaitingTimeFinished);

	/* Spawn Enemy */
	SpawnWaitTime = 2.f;
	RandomDeviation = 1.f;
	MaxSpawnCount = 3;
	SpawnDelayTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::AsyncSpawnEnemies);
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
	PrepareEnemySpawn();
	BroadcastStageStatusChangeToAllLocalPlayers();
	AsyncSpawnEnemies();
}

void AStageGameMode::EndStage()
{
	// 특정 조건을 만족해 호출됨 (모든 적 제거)

	StageStatus = EStageStatus::Waiting;
	StageNumber++;

	if (StageNumber >= MaxStageNumber)
	{
		// TODO : 게임 종료
		UE_LOG(LogTemp, Warning, TEXT("게임 종료"));
	}
	else
	{
		WaitStageStart();
	}
}

void AStageGameMode::InitData()
{
	// Caching MaxStageNumber
	check(StageConfig);
	MaxStageNumber = StageConfig->GetMaxStageNumber();
	check(MaxStageNumber > 0);

	// Caching SpawnEnemyVolumeExtent
	if (const ASpawnEnemyVolume* Volume = Cast<ASpawnEnemyVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnEnemyVolume::StaticClass())))
	{
		const FVector Location = Volume->BoxComponent->GetComponentLocation();
		const FVector BoxExtent = Volume->BoxComponent->GetScaledBoxExtent();
		SpawnEnemyVolumeBox = FBox(Location - BoxExtent, Location + BoxExtent);
	}
	
	// Caching SpawnParams for spawning enemies, beacons
	// Actor의 Ownership 설정을 위해 SimulatedProxy가 사용하는 PlayerController를 Owner로 설정
	SpawnParams.Owner = GetSimulatedPlayerController();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
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

	InitData();
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
	
	SpawnParams.bDeferConstruction = false;
	StartStageBeacon = GetWorld()->SpawnActor<AActor>(StartStageBeaconClass, SpawnParams);
}

void AStageGameMode::AsyncSpawnEnemies()
{
	check(StageConfig);

	if (NumSpawnedEnemies < RandomEnemyInfos.Num())
	{
		// 소환할 수 결정
		const int32 RandCount = FMath::RandRange(1, MaxSpawnCount);
		const int32 SpawnCount = FMath::Min(RandomEnemyInfos.Num() - NumSpawnedEnemies, RandCount);
		const int32 TotalCount = NumSpawnedEnemies + SpawnCount;

		// SpawnCount 만큼 소환
		for (; NumSpawnedEnemies < TotalCount; ++NumSpawnedEnemies)
		{
			const TSubclassOf<AAuraEnemy>& EnemyClass = EnemyClassTable[RandomEnemyInfos[NumSpawnedEnemies]];
			SpawnEnemy(EnemyClass);
		}

		// RandomDelay가 지난 뒤 다시 소환
		GetWorldTimerManager().SetTimer(SpawnDelayTimerHandle, SpawnDelayTimerDelegate, GetRandomDelay(), false);
	}
	else
	{
		// 모든 Enemy를 소환하면 스테이지를 종료할 수 있도록 설정
		bFinishSpawn = true;
	}
}

void AStageGameMode::SpawnEnemy(TSubclassOf<AAuraEnemy> Class)
{
	check(Class);

	SpawnParams.bDeferConstruction = true;
	
	// Find Random Point
	const FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox_Box(SpawnEnemyVolumeBox);
	const FTransform SpawnTransform(RandomPoint);

	if (AAuraEnemy* Enemy = GetWorld()->SpawnActor<AAuraEnemy>(Class, SpawnTransform, SpawnParams))
	{
		Enemy->SpawnDefaultController();
		Enemy->OnCharacterDeadDelegate.AddDynamic(this, &ThisClass::OnEnemyDead);
		Enemy->FinishSpawning(SpawnTransform);
	}
}

void AStageGameMode::OnEnemyDead()
{
	++NumDeadEnemies;
	if (bFinishSpawn && NumSpawnedEnemies == NumDeadEnemies)
	{
		// 모든 Enemy를 소환하고, 모든 Enemy가 죽으면 스테이지 종료
		EndStage();
	}
}

void AStageGameMode::PrepareEnemySpawn()
{
	bFinishSpawn = false;
	NumSpawnedEnemies = NumDeadEnemies = 0;
	
	RandomEnemyInfos.Empty();
	EnemyClassTable.Empty();

	// 소환해야 할 모든 Enemy 정보를 하나의 배열에 저장하고 Shuffle
	const TArray<FSpawnEnemyInfo>& SpawnEnemyInfoArray = StageConfig->GetSpawnEnemyInfosForStageNumberChecked(StageNumber);
	for (uint8 Index = 0; Index < SpawnEnemyInfoArray.Num(); ++Index)
	{
		// SpawnEnemyInfoArray의 Index를 해당 EnemyClass를 나타내는 uint8 값으로 사용한다.
		EnemyClassTable.Add(Index, SpawnEnemyInfoArray[Index].EnemyClass);
		for (int32 Count = 0; Count < SpawnEnemyInfoArray[Index].SpawnCount; ++Count)
		{
			RandomEnemyInfos.Add(Index);
		}
	}
	Algo::RandomShuffle(RandomEnemyInfos);
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
				AuraPC->MulticastOnStageStatusChanged(StageStatus, StageNumber, WaitingTimerEndSeconds, RandomEnemyInfos.Num());
			}
		}
	}
}
