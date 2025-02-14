// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Character/AuraEnemy.h"
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
		CostCastFailIndicatorComponent->SetRelativeLocation(FVector(0.f, 0.f, GetPawn()->GetDefaultHalfHeight() + 10.f));
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
		CooldownCastFailIndicatorComponent->SetRelativeLocation(FVector(0.f, 0.f, GetPawn()->GetDefaultHalfHeight() + 10.f));
		CooldownCastFailIndicatorComponent->RegisterComponent();
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

void AAuraPlayerController::SetInGameInputMode()
{
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}

void AAuraPlayerController::SetUIInputMode()
{
	const FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
}

void AAuraPlayerController::MulticastOnStageStatusChanged_Implementation(EStageStatus StageStatus, int32 StageNumber, double WaitingTimerEndSeconds, int32 TotalEnemyCount)
{
	if (const AAuraHUD* AuraHUD = GetHUD<AAuraHUD>())
	{
		AuraHUD->OnStageStatusChanged(StageStatus, StageNumber, WaitingTimerEndSeconds, TotalEnemyCount);
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

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	const AAuraGameStateBase* AuraGameStateBase = GetWorld() ? GetWorld()->GetGameState<AAuraGameStateBase>() : nullptr;
	check(AuraGameStateBase && AuraGameStateBase->AuraContext);
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(AuraGameStateBase->AuraContext, 0);
	}

	bShowMouseCursor = true;
	SetInGameInputMode();
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (HasAuthority())
	{
		BindAbilityInput();
	}
	else
	{
		GetWorldTimerManager().SetTimer(PollingTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::PollInit), 0.1f,true);
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

			// Notify for client
			OnGameStateBaseValidInClientDelegate.Broadcast();
		}
	}
}

void AAuraPlayerController::AbilityInputPressed(FGameplayTag InputTag, int32 InputID)
{
	if (InputTag == FAuraGameplayTags::Get().InputTag_LMB)
	{
		// TargetActor가 Enemy가 아니면 InputTag_LMB Abort
		if (TargetFromCurrentFrame.IsValid() && !TargetFromCurrentFrame->IsA<AAuraEnemy>())
		{
			return;
		}
	}
	else if (InputTag == FAuraGameplayTags::Get().InputTag_Interact)
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
