// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Character/AuraEnemy.h"
#include "Framework/Application/NavigationConfig.h"
#include "Game/AuraGameStateBase.h"
#include "Input/AuraInputComponent.h"
#include "Interface/InteractionInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/DamageIndicatorComponent.h"

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	// only called if the PlayerController has a PlayerInput object. it will not be called in non-locally controlled PlayerController.
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if (GetPawn() && bValidGameStateBaseInClient)
	{
		// Pawn이 설정되어 클라이언트로 Replicate 될 때
		// 이미 GameStateBase가 유효한 시점이라면 Level Sequence Actor를 Pawn에 부착한다.
		// 아직 GameStateBase가 유효하지 않은 첫 초기화 시점이라면 PollInit()에서 수행한다.
		AttachPauseMenuLevelSequenceActorToPawn();
	}
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

void AAuraPlayerController::AddUIMappingContext() const
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(UIContext, 1);
		Subsystem->RemoveMappingContext(AbilityContext);
	}
}

void AAuraPlayerController::RemoveUIMappingContext() const
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(UIContext);
		Subsystem->AddMappingContext(AbilityContext, 0);
	}
}

void AAuraPlayerController::CursorTrace()
{
	// Caching Target HitResult
	GetHitResultUnderCursor(ECC_Target, false, TargetHitResult);

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

void AAuraPlayerController::OnLevelSequencePlayerStop()
{
	SetViewTarget(GetPawn());
}

void AAuraPlayerController::ClientOnStageStatusChanged_Implementation(EStageStatus StageStatus, int32 StageNumber, double WaitingTimerEndSeconds, int32 TotalEnemyCount)
{
	if (OnStageStatusChangedDelegate.IsBound())
	{
		OnStageStatusChangedDelegate.Execute(StageStatus, StageNumber, WaitingTimerEndSeconds, TotalEnemyCount);
	}
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
	if (HasAuthority())
	{
		BindAbilityInput();
	}
	else
	{
		GetWorldTimerManager().SetTimer(PollingTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::PollInit), 0.1f,true);
	}

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
}

void AAuraPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 서버에서 Level Sequence Actor를 Pawn에 부착한다.
	AttachPauseMenuLevelSequenceActorToPawn();
}

void AAuraPlayerController::AttachPauseMenuLevelSequenceActorToPawn() const
{
	if (IsLocalController())
	{
		if (const AAuraGameStateBase* AuraGameStateBase = GetWorld() ? GetWorld()->GetGameState<AAuraGameStateBase>() : nullptr)
		{
			AuraGameStateBase->AttachPauseMenuLevelSequenceActorToPawn(GetPawn());
		}
	}
}

void AAuraPlayerController::PollInit()
{
	if (!HasAuthority() && !bValidGameStateBaseInClient)
	{
		if (GetWorld() && GetWorld()->GetGameState())
		{
			bValidGameStateBaseInClient = true;
			BindAbilityInput();
			GetWorldTimerManager().ClearTimer(PollingTimerHandle);

			// 클라이언트에서 초기화 시 Level Sequence Actor를 Pawn에 부착한다.
			// 이후부터는 OnRep_Pawn에서 수행한다.
			AttachPauseMenuLevelSequenceActorToPawn();

			// Notify for client
			OnGameStateBaseValidInClientDelegate.Broadcast();
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

void AAuraPlayerController::AbilityInputPressed(FGameplayTag InputTag, int32 InputID)
{
	if (InputTag == AuraGameplayTags::InputTag_LMB)
	{
		// TargetActor가 Enemy가 아니면 InputTag_LMB Abort
		if (TargetFromCurrentFrame.IsValid() && !TargetFromCurrentFrame->IsA<AAuraEnemy>())
		{
			return;
		}
	}
	else if (InputTag == AuraGameplayTags::InputTag_Interact)
	{
		// TargetActor가 없거나 Enemy면 InputTag_Interact Abort
		if (!TargetFromCurrentFrame.IsValid() || TargetFromCurrentFrame->IsA<AAuraEnemy>())
		{
			return;
		}
	}
	
	if (GetAuraAbilitySystemComponent())
	{
		AuraAbilitySystemComponent->AbilityInputPressed(InputID);
	}
}

void AAuraPlayerController::AbilityInputReleased(FGameplayTag InputTag, int32 InputID)
{
	if (GetAuraAbilitySystemComponent())
	{
		AuraAbilitySystemComponent->AbilityInputReleased(InputID);
	}
}

void AAuraPlayerController::BindAbilityInput()
{
	const AAuraGameStateBase* AuraGameStateBase = GetWorld() ? GetWorld()->GetGameState<AAuraGameStateBase>() : nullptr;
	if (AuraGameStateBase && AuraGameStateBase->AuraInputConfig)
	{
		/* Bind Action */
		UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
		AuraInputComponent->BindAbilityActions(AuraGameStateBase->AuraInputConfig, this, &ThisClass::AbilityInputPressed, &ThisClass::AbilityInputReleased);
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraAbilitySystemComponent()
{
	if (!AuraAbilitySystemComponent)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AuraAbilitySystemComponent;
}
