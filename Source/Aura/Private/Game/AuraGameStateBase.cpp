// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AuraGameStateBase.h"

#include "AuraBlueprintLibrary.h"
#include "Actor/Beacon_StartStage.h"
#include "Game/AuraGameInstance.h"
#include "Interface/PlayerInterface.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraLocalPlayer.h"
#include "UI/HUD/AuraHUD.h"

AAuraGameStateBase::AAuraGameStateBase()
{
	/* Respawn */
	TotalLifeCount = 3;
	RespawnTime = 7.f;

	/* Beacon */
	BeaconSpawnDistance = 500.f;
}

void AAuraGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraGameStateBase, StartStageBeacon);
}

/* Called on Server Only */
void AAuraGameStateBase::SpawnStartStageBeacon()
{
	if (IsValid(StartStageBeacon))
	{
		StartStageBeacon->Destroy();
	}
	
	// Spawn Location 결정
	FVector SpawnLocation(0.f);
	
	TArray<AActor*> AlivePawns;
	UAuraBlueprintLibrary::GetAlivePawnsFromPlayers(GetWorld(), AlivePawns);
	if (AlivePawns.Num())
	{
		// 살아있는 플레이어를 기준으로 Beacon 위치 계산
		if (const AActor* AlivePawn = AlivePawns[0])
		{
			const FVector Direction = AlivePawn->GetActorLocation().GetSafeNormal();	// to 0, 0, 0
			SpawnLocation = AlivePawn->GetActorLocation() + -Direction * BeaconSpawnDistance;
			SpawnLocation.Z = 0.f;
		}
	}

	// Actor의 Ownership 설정을 위해 SimulatedProxy PlayerController를 Owner로 설정
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = UAuraBlueprintLibrary::GetSimulatedPlayerController(GetWorld());
	StartStageBeacon = GetWorld()->SpawnActor<ABeacon_StartStage>(StartStageBeaconClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
	
	PlaySpawnBeaconLevelSequence();
}

/* Called on Server Only */
void AAuraGameStateBase::DestroyStartStageBeacon() const
{
	if (IsValid(StartStageBeacon))
	{
		StartStageBeacon->Destroy();
	}
}

bool AAuraGameStateBase::IsStartStageBeaconValid() const
{
	return IsValid(StartStageBeacon);
}

void AAuraGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	check(StartStageBeaconClass);

	// 게임이 시작하고 모든 플레이어가 연결되는 딜레이를 가리기 위해 BlackScreenOverlay 표시
	if (UAuraGameInstance* AuraGameInstance = GetGameInstance<UAuraGameInstance>())
	{
		AuraGameInstance->ShowBlackScreenOverlay(true);
	}

	// 입력 비활성화
	if (APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
	{
		PC->DisableInput(PC);
	}
}

void AAuraGameStateBase::OnRep_StartStageBeacon()
{
	PlaySpawnBeaconLevelSequence();
}

void AAuraGameStateBase::PlaySpawnBeaconLevelSequence()
{
	if (!IsValid(StartStageBeacon))
	{
		return;
	}
	
	APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController<APlayerController>() : nullptr;
	
	// Hide HUD
	if (const AAuraHUD* AuraHUD = PlayerController ? PlayerController->GetHUD<AAuraHUD>() : nullptr)
	{
		AuraHUD->CloseAllMenu();
		AuraHUD->ShowGameOverlay(false);
	}

	// Play
	if (IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(PlayerController))
	{
		// Add Delegate
		if (FOnLevelSequenceStopSignature* OnLevelSequenceStopDelegate = PlayerInterface->GetOnLevelSequenceStopDelegate())
		{
			if (!OnLevelSequenceStopDelegate->IsBoundToObject(this))
			{
				OnLevelSequenceStopDelegate->AddUObject(this, &ThisClass::OnLevelSequenceStop);
			}
		}
		FVector NewLocation = StartStageBeacon->GetActorLocation();
		NewLocation.Z = 0.f;
		PlayerInterface->SetLevelSequenceActorLocation(TEXT("SpawnBeacon"), NewLocation);
		PlayerInterface->PlayLevelSequence(TEXT("SpawnBeacon"));
		PlayerInterface->EnableCinematicInput();
	}

	// 처음 게임에 진입하면 BlackScreenOverlay가 표시되어 있으므로 Level Sequence를 재생하기 전에 숨김
	// 까맣게 변경한 화면을 원래대로 되돌린다.
	if (bWaitingForFirstLevelSequenceStop)	
	{
		if (UAuraGameInstance* AuraGameInstance = GetGameInstance<UAuraGameInstance>())
		{
			AuraGameInstance->ShowBlackScreenOverlay(false);
		}
		if (UAuraLocalPlayer* AuraLocalPlayer = GetWorld() ? GetWorld()->GetFirstLocalPlayerFromController<UAuraLocalPlayer>() : nullptr)
		{
			AuraLocalPlayer->SetSceneViewColorBlack(false);
		}

		// 입력 활성화
		if (APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
		{
			PC->EnableInput(PC);
		}
	}
}

void AAuraGameStateBase::OnLevelSequenceStop(const FName& LevelSequenceTag)
{
	if (LevelSequenceTag != TEXT("SpawnBeacon"))
	{
		return;
	}
	
	if (StartStageBeacon)
	{
		StartStageBeacon->SetMeshComponentHiddenInGame(false);
	}

	APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController<APlayerController>() : nullptr;

	// Show HUD
	if (const AAuraHUD* AuraHUD = PlayerController ? PlayerController->GetHUD<AAuraHUD>() : nullptr)
	{
		AuraHUD->ShowGameOverlay(true);
	}

	if (IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(PlayerController))
	{
		PlayerInterface->DisableCinematicInput();
		PlayerInterface->FlushPendingStageWaitingBroadcast();

		// 처음 한번만 Initial Logic 처리
		if (bWaitingForFirstLevelSequenceStop)
		{
			bWaitingForFirstLevelSequenceStop = false;
			PlayerInterface->HandleInitialLogic();
		}
	}
}
