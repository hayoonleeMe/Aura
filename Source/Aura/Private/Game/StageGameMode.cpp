// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/StageGameMode.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraBlueprintLibrary.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Actor/SpawnEnemyVolume.h"
#include "Algo/RandomShuffle.h"
#include "Character/AuraEnemy.h"
#include "Components/BoxComponent.h"
#include "Data/StageConfig.h"
#include "Game/AuraGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/AuraPlayerController.h"
#include "AbilitySystem/AuraAttributeSet.h"

// For Test in PIE
#if WITH_EDITOR
#include "Settings/LevelEditorPlaySettings.h"
#endif

AStageGameMode::AStageGameMode()
{
	/* Waiting Timer */
	WaitingTime = 10.f;
	WaitingTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::OnWaitingTimeFinished);
	
	/* End Stage Delay Timer */
	EndStageDelay = 4.f;
	EndStageDelayTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::EndStage);

	/* Spawn Enemy */
	SpawnWaitTime = 2.f;
	RandomDeviation = 1.f;
	MaxSpawnCountPerBatch = 3;
	SpawnDelayTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::AsyncSpawnEnemies);

	/* End Game */
	GameEndDelaySeconds = 5.f;
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

void AStageGameMode::RequestPlayerRespawn(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid PlayerController in %hs"), __FUNCTION__);
		return;
	}
	
	AAuraPlayerController* AuraPC = CastChecked<AAuraPlayerController>(PlayerController);
	AuraPC->UseLifeCount();
	
	const bool bGameEnd = TotalLifeCount <= AuraPC->GetUsedLifeCount();
	NotifyRespawnStartToLocalPlayer(PlayerController, bGameEnd);

	if (bGameEnd)
	{
		// Can't Respawn, Try End Game
		HandlePlayerRetire();
	}
	else
	{
		// Start Respawn Timer
		FTimerHandle RespawnTimerHandle;
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::OnRespawnTimerFinished, PlayerController), RespawnTime, false);	
	}
}

void AStageGameMode::OnRespawnTimerFinished(APlayerController* ControllerToRespawn)
{
	APawn* PawnToRespawn = ControllerToRespawn ? ControllerToRespawn->GetPawn() : nullptr;
	if (!IsValid(ControllerToRespawn) || !IsValid(PawnToRespawn))
	{
		return;
	}

	// Uninitialize ASC
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PawnToRespawn))
	{
		AbilitySystemComponent->RemoveAllGameplayCues();
		AbilitySystemComponent->SetAvatarActor(nullptr);
	}

	const FTransform RespawnTransform = PawnToRespawn->GetActorTransform();

	ControllerToRespawn->Reset();
	PawnToRespawn->SetActorHiddenInGame(true);
	PawnToRespawn->DetachFromControllerPendingDestroy();
	PawnToRespawn->Destroy();

	// Restart Next Frame
	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this, ControllerToRespawn, RespawnTransform]()
	{
		RestartPlayerAtTransform(ControllerToRespawn, RespawnTransform);
	}));
}

void AStageGameMode::OnPlayerReady()
{
	++ReadyPlayerCount;

	if (GetWorld() && ReadyPlayerCount == GetWorld()->GetNumPlayerControllers())
	{
		// 모든 플레이어 레디 완료
		StartStage();
		DestroyStartStageBeacon();
	}
}

void AStageGameMode::SpawnStartStageBeacon() const
{
	if (StageGameState)
	{
		StageGameState->SpawnStartStageBeacon();
	}
}

void AStageGameMode::DestroyStartStageBeacon() const
{
	if (StageGameState)
	{
		StageGameState->DestroyStartStageBeacon();
	}
}

void AStageGameMode::HandlePlayerRetire()
{
	++RetiredPlayerCount;
	if (RetiredPlayerCount == GetWorld()->GetNumPlayerControllers())
	{
		EndGame();
	}
}

void AStageGameMode::EndGame() const
{
	// 게임 종료를 UI에 표시
	BroadcastGameEndToAllLocalPlayers();

	// 등록된 모든 타이머 제거
	GetWorldTimerManager().ClearAllTimersForObject(this);

	// DestroySession이 끝날 때 콜백 함수 OnDestroySessionComplete 등록
	FTimerHandle EndGameTimerHandle;
	GetWorldTimerManager().SetTimer(EndGameTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		if (MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->DestroySession();
		}
	}), GameEndDelaySeconds, false);
}

void AStageGameMode::OnDestroySessionComplete(bool bWasSuccessful) const
{
	if (bWasSuccessful)
	{
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			GameInstance->ReturnToMainMenu();
		}
	}
	else
	{
		// 세션 제거에 실패하면 다시 시도
		if (MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->DestroySession();
		}
	}
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

	if (StageNumber > MaxStageNumber)
	{
		EndGame();
	}
	else
	{
		WaitStageStart();
	}
}

void AStageGameMode::OnStageBeaconInteracted()
{
	if (StageNumber > 1)
	{
		StartStage();
		DestroyStartStageBeacon();
	}
	else
	{
		// stage 시작 전 초반
		OnPlayerReady();
	}
}

void AStageGameMode::InitData()
{
	if (!GetWorld())
	{
		return;
	}
	
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
	
	// Actor의 Ownership 설정을 위해 SimulatedProxy PlayerController를 Owner로 설정
	EnemySpawnParams.Owner = UAuraBlueprintLibrary::GetSimulatedPlayerController(GetWorld());
	EnemySpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	EnemySpawnParams.bDeferConstruction = true;

	// Caching Game State Base
	StageGameState = GetGameState<AAuraGameStateBase>();
	check(StageGameState);
	
	// Caching Respawn Data
	if (StageGameState)
	{
		TotalLifeCount = StageGameState->GetTotalLifeCount();
		RespawnTime = StageGameState->GetRespawnTime();
	}

	// Caching MultiplayerSessionsSubsystem and Bind Callback
	if (const UGameInstance* GameInstance = GetWorld()->GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		if (MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->AuraOnDestroySessionCompleteDelegate.AddUObject(this, &ThisClass::OnDestroySessionComplete);
		}
	}

	// EnemySpawnLevel을 업데이트하기 위해 플레이어 레벨이 변경될 때 콜백 함수 등록
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (It->IsValid())
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(It->Get()->GetPawn()))
			{
				ASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetLevelAttribute()).AddUObject(this, &ThisClass::OnPlayerLevelAttributeChanged);
			}
		}
	}
}

void AStageGameMode::PollInit()
{
	if (ASCInitializedPlayers.Num() != GetWorld()->GetNumPlayerControllers())
	{
		return;
	}
	
	GetWorldTimerManager().ClearTimer(PollingTimerHandle);

	InitData();
	SpawnStartStageBeacon();
}

void AStageGameMode::StartWaitingTimer()
{
	GetWorldTimerManager().SetTimer(WaitingTimerHandle, WaitingTimerDelegate, WaitingTime, false);
}

void AStageGameMode::OnWaitingTimeFinished()
{
	DestroyStartStageBeacon();
	StartStage();
}

void AStageGameMode::AsyncSpawnEnemies()
{
	check(StageConfig);

	const int32 MaxStageEnemyCount = RandomEnemyInfos.Num();
	if (NumStageSpawnedEnemies < MaxStageEnemyCount)
	{
		// 소환할 수 결정
		const int32 RandCount = FMath::RandRange(1, MaxSpawnCountPerBatch);
		const int32 SpawnCount = FMath::Min(MaxStageEnemyCount - NumStageSpawnedEnemies, RandCount);

		// SpawnCount 만큼 소환
		for (int32 Index = 0; Index < SpawnCount; ++Index)
		{
			const TSubclassOf<AAuraEnemy>& EnemyClass = EnemyClassTable[RandomEnemyInfos[NumStageSpawnedEnemies]];
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

void AStageGameMode::OnPlayerLevelAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (GetWorld())
	{
		// 모든 플레이어의 평균 레벨 증가량을 반영하여 EnemySpawnLevel 업데이트
		const float AddAmount = 1.f / GetWorld()->GetNumPlayerControllers();
		EnemySpawnLevel += AddAmount;
	}
}

void AStageGameMode::SpawnEnemy(TSubclassOf<AAuraEnemy> Class)
{
	check(Class);

	
	// Find Random Point
	const FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox_Box(SpawnEnemyVolumeBox);
	const FTransform SpawnTransform(RandomPoint);

	if (AAuraEnemy* Enemy = GetWorld()->SpawnActor<AAuraEnemy>(Class, SpawnTransform, EnemySpawnParams))
	{
		++NumStageSpawnedEnemies;

		Enemy->SpawnDefaultController();
		Enemy->OnCharacterDeadDelegate.AddDynamic(this, &ThisClass::OnEnemyDead);
		Enemy->SpawnLevel = FMath::RoundToInt32(EnemySpawnLevel);
		Enemy->FinishSpawning(SpawnTransform);
	}
}

void AStageGameMode::RequestSpawnEnemy(const TSubclassOf<AAuraEnemy>& EnemyClass, FTransform SpawnTransform, bool bOverrideLocationZ)
{
	check(EnemyClass);

	FVector SpawnLocation = SpawnTransform.GetTranslation();
	SpawnLocation.Z = bOverrideLocationZ ? SpawnEnemyVolumeBox.Min.Z : SpawnLocation.Z;
	SpawnTransform.SetTranslation(SpawnLocation);

	if (AAuraEnemy* Enemy = GetWorld()->SpawnActor<AAuraEnemy>(EnemyClass, SpawnTransform, EnemySpawnParams))
	{
		++TotalEnemyCount;
		
		Enemy->SpawnDefaultController();
		Enemy->OnCharacterDeadDelegate.AddDynamic(this, &ThisClass::OnEnemyDead);
		Enemy->SpawnLevel = FMath::RoundToInt32(EnemySpawnLevel);
		Enemy->FinishSpawning(SpawnTransform);

		BroadcastTotalEnemyCountChangeToAllLocalPlayers();
	}
}

void AStageGameMode::OnPlayerASCInitialized(APlayerController* PC)
{
	ASCInitializedPlayers.Add(PC);
}

void AStageGameMode::OnEnemyDead()
{
	++NumDeadEnemies;
	if (bFinishSpawn && NumDeadEnemies == TotalEnemyCount)
	{
		// 모든 Enemy를 소환하고, 모든 Enemy가 죽으면 EndStageDelay가 지난 뒤 스테이지 종료
		GetWorldTimerManager().SetTimer(EndStageDelayTimerHandle, EndStageDelayTimerDelegate, EndStageDelay, false);
	}
}

void AStageGameMode::PrepareEnemySpawn()
{
	bFinishSpawn = false;
	NumStageSpawnedEnemies = NumDeadEnemies = 0;
	
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
	
	TotalEnemyCount = RandomEnemyInfos.Num();
}

void AStageGameMode::BroadcastStageStatusChangeToAllLocalPlayers() const
{
	if (GetWorld())
	{
		const double WaitingTimerEndSeconds = StageStatus == EStageStatus::Waiting ? GetWorld()->GetTimeSeconds() + WaitingTime : 0.0;

		// 모든 Player Controller의 Client RPC를 호출해 로컬 플레이어의 화면에 표시
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(It->Get()))
			{
				AuraPC->ClientOnStageStatusChanged(StageStatus, StageNumber, WaitingTimerEndSeconds, TotalEnemyCount);
			}
		}
	}
}

void AStageGameMode::BroadcastTotalEnemyCountChangeToAllLocalPlayers() const
{
	if (GetWorld())
	{
		// 모든 Player Controller의 Client RPC를 호출해 로컬 플레이어의 화면에 표시
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(It->Get()))
			{
				AuraPC->ClientOnTotalEnemyCountChanged(TotalEnemyCount);
			}
		}
	}
}

void AStageGameMode::NotifyRespawnStartToLocalPlayer(APlayerController* ControllerToRespawn, bool bGameEnd) const
{
	const double RespawnTimerEndSeconds = !bGameEnd ? GetWorld()->GetTimeSeconds() + RespawnTime : -1.f;

	// 로컬 플레이어 화면에 표시
	if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(ControllerToRespawn))
	{
		AuraPC->ClientNotifyRespawnStart(RespawnTimerEndSeconds);
	}
}

void AStageGameMode::BroadcastGameEndToAllLocalPlayers() const
{
	if (GetWorld())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(It->Get()))
			{
				AuraPC->ClientEndGame();
			}
		}
	}
}
