// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Component/LevelSequenceManageComponent.h"
#include "Framework/Application/NavigationConfig.h"
#include "Game/AuraGameStateBase.h"
#include "Input/AuraInputComponent.h"
#include "Interface/InteractionInterface.h"
#include "Interface/StageSystemInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/DamageIndicatorComponent.h"
#include "GameFramework/GameModeBase.h"

AAuraPlayerController::AAuraPlayerController()
{
	LevelSequenceManageComponent = CreateDefaultSubobject<ULevelSequenceManageComponent>(TEXT("Level Sequence Manage Component"));
	LevelSequenceManageComponent->SetLevelSequenceTags({TEXT("PauseMenu"), TEXT("SpawnBeacon")});
	LevelSequenceManageComponent->OnLevelSequenceStopDelegate.AddUObject(this, &ThisClass::OnLevelSequencePlayerStop);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	// only called if the PlayerController has a PlayerInput object. it will not be called in non-locally controlled PlayerController.
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	AttachPauseMenuLevelSequenceActorToPawn();
}

void AAuraPlayerController::IndicateAbilityActivateCostFail()
{
	// 생성된 WidgetComponent가 없을 때 다시 생성해 표시
	if (IsValid(CostCastFailIndicatorComponent) || !CostCastFailIndicatorComponentClass || !IsValid(GetPawn()))
	{
		return;
	}

	CostCastFailIndicatorComponent = NewObject<UWidgetComponent>(GetPawn(), CostCastFailIndicatorComponentClass);
	if (CostCastFailIndicatorComponent)
	{
		CostCastFailIndicatorComponent->SetupAttachment(GetPawn()->GetRootComponent());
		CostCastFailIndicatorComponent->SetRelativeLocation(FVector(0.f, 0.f, GetPawn()->GetDefaultHalfHeight() * 2.f));
		CostCastFailIndicatorComponent->RegisterComponent();
	}
}

void AAuraPlayerController::IndicateAbilityActivateCooldownFail()
{
	// 생성된 WidgetComponent가 없을 때 다시 생성해 표시
	if (IsValid(CooldownCastFailIndicatorComponent) || !CooldownCastFailIndicatorComponentClass || !IsValid(GetPawn()))
	{
		return;
	}

	CooldownCastFailIndicatorComponent = NewObject<UWidgetComponent>(GetPawn(), CooldownCastFailIndicatorComponentClass);
	if (CooldownCastFailIndicatorComponent)
	{
		CooldownCastFailIndicatorComponent->SetupAttachment(GetPawn()->GetRootComponent());
		CooldownCastFailIndicatorComponent->SetRelativeLocation(FVector(0.f, 0.f, GetPawn()->GetDefaultHalfHeight() * 2.f));
		CooldownCastFailIndicatorComponent->RegisterComponent();
	}
}

void AAuraPlayerController::NotifyEnemyDead()
{
	if (OnEnemyDeadDelegate.IsBound())
	{
		OnEnemyDeadDelegate.Execute();
	}
}

void AAuraPlayerController::EnableUIInput()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(UIContext, 1);
	}
	DisableAbilityInput();

	EnableHighlight(false);
}

void AAuraPlayerController::DisableUIInput()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(UIContext);
	}
	EnableAbilityInput();

	EnableHighlight(true);
}

void AAuraPlayerController::EnableCinematicInput()
{
	StoreInputMappingContextState();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		
		++CinematicInputAddCount;
		Subsystem->AddMappingContext(CinematicContext, 0);
	}
	
	EnableHighlight(false);
}

void AAuraPlayerController::DisableCinematicInput()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{	
		--CinematicInputAddCount;
		if (CinematicInputAddCount == 0)
		{
			Subsystem->RemoveMappingContext(CinematicContext);
		}
	}
	RestoreInputMappingContextState();

	EnableHighlight(true);
}

FKey AAuraPlayerController::GetInteractKeyMappedToAction() const
{
	const TArray<FEnhancedActionKeyMapping>& Mappings = AbilityContext->GetMappings();
	for (const FEnhancedActionKeyMapping& Mapping : Mappings)
	{
		if (Mapping.Action == AuraInputConfig->GetInputActionForInputTag(AuraGameplayTags::InputTag_Interact))
		{
			return Mapping.Key;
		}
	}
	return FKey();
}

FOnLevelSequenceStopSignature* AAuraPlayerController::GetOnLevelSequenceStopDelegate() const
{
	if (LevelSequenceManageComponent)
	{
		return &LevelSequenceManageComponent->OnLevelSequenceStopDelegate;
	}
	return nullptr;
}

void AAuraPlayerController::PlayLevelSequence(const FName& LevelSequenceTag)
{
	EnableCachingTargetHitResult(false);
	
	if (LevelSequenceManageComponent)
	{
		LevelSequenceManageComponent->PlayLevelSequence(LevelSequenceTag);
	}
}

void AAuraPlayerController::StopLevelSequence(const FName& LevelSequenceTag)
{
	if (LevelSequenceManageComponent)
	{
		LevelSequenceManageComponent->StopLevelSequence(LevelSequenceTag);
	}
}

void AAuraPlayerController::SetLevelSequenceActorLocation(const FName& LevelSequenceTag, const FVector& NewLocation)
{
	if (LevelSequenceManageComponent)
	{
		LevelSequenceManageComponent->SetLevelSequenceActorLocation(LevelSequenceTag, NewLocation);
	}
}

void AAuraPlayerController::HandleInitialLogic() const
{
	// GameOverlay Widget 초기화
	if (AAuraHUD* AuraHUD = GetHUD<AAuraHUD>())
	{
		AuraHUD->InitOverlay();
	}
	ShowStageReadyHUD();
	ShowAllMenuShortcutAlert();
}

void AAuraPlayerController::ShowStageReadyHUD() const
{
	if (const AAuraHUD* AuraHUD = GetHUD<AAuraHUD>())
	{
		AuraHUD->ShowStageReadyHUD();
	}
}

void AAuraPlayerController::ShowAllMenuShortcutAlert() const
{
  	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	const AAuraHUD* AuraHUD = GetHUD<AAuraHUD>();
	if (Subsystem && AuraHUD)
	{
		// 유효한 Key Query를 위해 즉시 RebuildControlMappings를 수행  
		FModifyContextOptions Options;
		Options.bForceImmediately = true;
		Subsystem->RequestRebuildControlMappings(Options);

		// 메뉴와 메뉴를 여는 Input Key
		TArray<TTuple<EGameMenuType, FKey>> MenuKeys;
		
		const TArray<FKey> TutorialMenuKeys(Subsystem->QueryKeysMappedToAction(IA_TutorialMenu));
		if (TutorialMenuKeys.Num())
		{
			MenuKeys.Add({EGameMenuType::TutorialMenu, TutorialMenuKeys[0]});
		}
		const TArray<FKey> AttributeMenuKeys(Subsystem->QueryKeysMappedToAction(IA_AttributeMenu));
		if (AttributeMenuKeys.Num())
		{
			MenuKeys.Add({EGameMenuType::AttributeMenu, AttributeMenuKeys[0]});
		}
		const TArray<FKey> SpellMenuKeys(Subsystem->QueryKeysMappedToAction(IA_SpellMenu));
		if (SpellMenuKeys.Num())
		{
			MenuKeys.Add({EGameMenuType::SpellMenu, SpellMenuKeys[0]});
		}
		const TArray<FKey> PauseMenuKeys(Subsystem->QueryKeysMappedToAction(IA_PauseMenu));
		if (PauseMenuKeys.Num())
		{
			MenuKeys.Add({EGameMenuType::PauseMenu, PauseMenuKeys[0]});
		}

		AuraHUD->ShowAllMenuShortcutAlert(MenuKeys);
	}
}

void AAuraPlayerController::EnableAbilityInput()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		--AbilityInputBlockCount;
		if (AbilityInputBlockCount == 0)
		{
			Subsystem->AddMappingContext(AbilityContext, 0);
		}
	}	
}

void AAuraPlayerController::DisableAbilityInput()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		++AbilityInputBlockCount;
		Subsystem->RemoveMappingContext(AbilityContext);
	}	
}

void AAuraPlayerController::ServerNotifyASCInitToGameMode_Implementation()
{
	if (IStageSystemInterface* StageSystemInterface = Cast<IStageSystemInterface>(GetWorld() ? GetWorld()->GetAuthGameMode() : nullptr))
	{
		StageSystemInterface->OnPlayerASCInitialized(this);
	}
}

void AAuraPlayerController::CursorTrace()
{
	if (bEnableCachingTargetHitResult)
	{
		// Caching Target HitResult
		GetHitResultUnderCursor(ECC_Target, false, TargetHitResult);
	}

	if (bEnableHighlight)
	{
		// Highlight Target
		TargetFromPrevFrame = TargetFromCurrentFrame;
		TargetFromCurrentFrame = IsValid(TargetHitResult.GetActor()) && TargetHitResult.GetActor()->Implements<UInteractionInterface>() ? TargetHitResult.GetActor() : nullptr;
		if (TargetFromPrevFrame != TargetFromCurrentFrame)
		{
			if (IInteractionInterface* PrevInteractionInterface = Cast<IInteractionInterface>(TargetFromPrevFrame))
			{
				PrevInteractionInterface->UnHighlightActor();
			}
			if (IInteractionInterface* CurrentInteractionInterface = Cast<IInteractionInterface>(TargetFromCurrentFrame))
			{
				CurrentInteractionInterface->HighlightActor();
			}		
		}	
	}
}

void AAuraPlayerController::EnableHighlight(bool bEnabled)
{
	if (bEnabled)
	{
		bEnableHighlight = true;
	}
	else
	{
		bEnableHighlight = false;
		if (IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(TargetFromCurrentFrame))
		{
			InteractionInterface->UnHighlightActor();
		}
		TargetFromCurrentFrame = nullptr;
		TargetFromPrevFrame = nullptr;
	}
}

void AAuraPlayerController::EnableCachingTargetHitResult(bool bEnabled)
{
	if (bEnabled)
	{
		GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			// 재생 중인 Level Sequence가 없을 때만 활성화
			if (!LevelSequenceManageComponent->IsPlayingLevelSequence())
			{
				// 다음 프레임에 수행해 잘못된 커서 위치에서 CursorTrace() 호출을 방지
				bEnableCachingTargetHitResult = true;
			}
		}));
	}
	else
	{
		bEnableCachingTargetHitResult = false;
		TargetHitResult = FHitResult();
	}
}

void AAuraPlayerController::ClientIndicateDamage_Implementation(float Damage, bool bIsBlockedHit, bool bIsCriticalHit, const FVector_NetQuantize& TargetLocation) const
{
	if (!DamageIndicatorComponentClass || !IsValid(GetPawn()))
	{
		return;
	}
	
	if (UDamageIndicatorComponent* DamageIndicatorComponent = NewObject<UDamageIndicatorComponent>(GetPawn(), DamageIndicatorComponentClass))
	{
		DamageIndicatorComponent->RegisterComponent();
		DamageIndicatorComponent->UpdateDamage(Damage, bIsBlockedHit, bIsCriticalHit);

		// Damage Indicator를 랜덤한 위치에 표시
		const FVector RandomLocation = TargetLocation + UKismetMathLibrary::RandomUnitVector() * FMath::RandRange(DamageIndicatorComponent->WidgetSpawnMinDist, DamageIndicatorComponent->WidgetSpawnMaxDist);
		DamageIndicatorComponent->SetWorldLocation(RandomLocation);
	}
}

void AAuraPlayerController::ClientOnStageStatusChanged_Implementation(EStageStatus StageStatus, int32 StageNumber, double WaitingTimerEndSeconds, int32 TotalEnemyCount)
{
	if (StageStatus == EStageStatus::Waiting)
	{
		if (const AAuraGameStateBase* AuraGameStateBase = GetWorld() ? GetWorld()->GetGameState<AAuraGameStateBase>() : nullptr)
		{
			if (AuraGameStateBase->IsStartStageBeaconValid())
			{
				if (OnStageStatusChangedDelegate.IsBound())
				{
					OnStageStatusChangedDelegate.Execute(StageStatus, StageNumber, WaitingTimerEndSeconds, TotalEnemyCount);
				}	
			}
			else
			{
				PendingStageWaitingBroadcastParams.Add({ StageNumber, WaitingTimerEndSeconds, TotalEnemyCount });
			}
		}
	}
	else
	{
		if (const AAuraGameStateBase* AuraGameStateBase = GetWorld() ? GetWorld()->GetGameState<AAuraGameStateBase>() : nullptr)
		{
			// Stage가 시작하면 SpawnBeacon Level Sequence 종료
			AuraGameStateBase->OnStageStarted();
		}
		if (OnStageStatusChangedDelegate.IsBound())
		{
			OnStageStatusChangedDelegate.Execute(StageStatus, StageNumber, WaitingTimerEndSeconds, TotalEnemyCount);
		}
	}
}

void AAuraPlayerController::FlushPendingStageWaitingBroadcast()
{
	for (const StageWaitingBroadcastParam& Param : PendingStageWaitingBroadcastParams)
	{
		if (OnStageStatusChangedDelegate.IsBound())
		{
			OnStageStatusChangedDelegate.Execute(EStageStatus::Waiting, Param.StageNumber, Param.WaitingTimerEndSeconds, Param.TotalEnemyCount);
		}	
	}
	PendingStageWaitingBroadcastParams.Empty();
}

void AAuraPlayerController::ClientOnTotalEnemyCountChanged_Implementation(int32 TotalEnemyCount)
{
	if (OnTotalEnemyCountChangedDelegate.IsBound())
	{
		OnTotalEnemyCountChangedDelegate.Execute(TotalEnemyCount);
	}
}

void AAuraPlayerController::ClientNotifyRespawnStart_Implementation(float RespawnTimerEndSeconds)
{
	OnRespawnStartedDelegate.Broadcast(RespawnTimerEndSeconds);
}

void AAuraPlayerController::ClientEndGame_Implementation()
{
	if (OnGameEndDelegate.IsBound())
	{
		OnGameEndDelegate.Execute();
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Disable Navigation
	FSlateApplication::Get().GetNavigationConfig()->bTabNavigation = false;
	FSlateApplication::Get().GetNavigationConfig()->bAnalogNavigation = false;
	FSlateApplication::Get().GetNavigationConfig()->bKeyNavigation = false;

	check(AbilityContext);
	check(CommonContext);
	check(UIContext);
	check(CinematicContext);
	check(AuraInputConfig);
	check(SpellConfig);
	check(AttributeConfig);
	
	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(AbilityContext, 0);
		Subsystem->AddMappingContext(CommonContext, 0);
	}

	bShowMouseCursor = true;
	
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Bind Ability Context
	BindAbilityInput();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	
	// Bind Common Context
	if (IA_AttributeMenu)
	{
		AuraInputComponent->BindAction(IA_AttributeMenu, ETriggerEvent::Started, this, &ThisClass::OnMenuActionStarted, EGameMenuType::AttributeMenu);
	}
	if (IA_SpellMenu)
	{
		AuraInputComponent->BindAction(IA_SpellMenu, ETriggerEvent::Started, this, &ThisClass::OnMenuActionStarted, EGameMenuType::SpellMenu);
	}
	if (IA_PauseMenu)
	{
		AuraInputComponent->BindAction(IA_PauseMenu, ETriggerEvent::Started, this, &ThisClass::OnMenuActionStarted, EGameMenuType::PauseMenu);
	}
	if (IA_TutorialMenu)
	{
		AuraInputComponent->BindAction(IA_TutorialMenu, ETriggerEvent::Started, this, &ThisClass::OnMenuActionStarted, EGameMenuType::TutorialMenu);
	}
	
	// Bind UI Context
	if (IA_CloseUI)
	{
		AuraInputComponent->BindAction(IA_CloseUI, ETriggerEvent::Started, this, &ThisClass::OnCloseUIActionStarted);
	}

	// Bind Cinematic Context
	if (IA_CloseCinematic)
	{
		AuraInputComponent->BindAction(IA_CloseCinematic, ETriggerEvent::Started, this, &ThisClass::OnCloseCinematicActionStarted);
	}
}

void AAuraPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsLocalController())
	{
		// Level Sequence Actor를 Pawn에 부착한다.
		AttachPauseMenuLevelSequenceActorToPawn();
	}
}

void AAuraPlayerController::StoreInputMappingContextState()
{
	if (const UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (Subsystem->HasMappingContext(CommonContext))
		{
			IMCFlags |= 1 << 0;
		}
		if (Subsystem->HasMappingContext(AbilityContext))
		{
			IMCFlags |= 1 << 1;
		}
		if (Subsystem->HasMappingContext(UIContext))
		{
			IMCFlags |= 1 << 2;
		}
	}
}

void AAuraPlayerController::RestoreInputMappingContextState()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (IMCFlags & 1 << 0)
		{
			Subsystem->AddMappingContext(CommonContext, 0);		// TODO : Priority
		}
		if (IMCFlags & 1 << 1)
		{
			Subsystem->AddMappingContext(AbilityContext, 0);
		}
		if (IMCFlags & 1 << 2)
		{
			Subsystem->AddMappingContext(UIContext, 1);
		}
	}
}

void AAuraPlayerController::OnMenuActionStarted(EGameMenuType GameMenuType)
{
	if (const AAuraHUD* AuraHUD = GetHUD<AAuraHUD>())
	{
		AuraHUD->OpenMenu(GameMenuType);
	}
}

void AAuraPlayerController::OnCloseUIActionStarted()
{
	if (ABaseHUD* BaseHUD = GetHUD<ABaseHUD>())
	{
		BaseHUD->CloseCurrentWidget();
	}
}

void AAuraPlayerController::OnCloseCinematicActionStarted()
{
	if (LevelSequenceManageComponent)
	{
		LevelSequenceManageComponent->StopCurrentPlayingLevelSequence();
	}
}

void AAuraPlayerController::AbilityInputPressed(int32 InputID, bool bUseTriggeredEvent)
{
	if (GetAuraAbilitySystemComponent())
	{
		AuraAbilitySystemComponent->AbilityInputPressed(InputID, bUseTriggeredEvent);
	}
}

void AAuraPlayerController::AbilityInputReleased(int32 InputID)
{
	if (GetAuraAbilitySystemComponent())
	{
		AuraAbilitySystemComponent->AbilityInputReleased(InputID);
	}
}

void AAuraPlayerController::BindAbilityInput()
{
	/* Bind Action */
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAbilityActions(AuraInputConfig, this, &ThisClass::AbilityInputPressed, &ThisClass::AbilityInputReleased);
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraAbilitySystemComponent()
{
	if (!AuraAbilitySystemComponent)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::AttachPauseMenuLevelSequenceActorToPawn() const
{
	if (LevelSequenceManageComponent)
	{
		LevelSequenceManageComponent->AttachLevelSequenceActorToPawn(TEXT("PauseMenu"), GetPawn(), true);
	}
}

void AAuraPlayerController::OnLevelSequencePlayerStop(const FName& LevelSequenceTag)
{
	EnableCachingTargetHitResult(true);
}
