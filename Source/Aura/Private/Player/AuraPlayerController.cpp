// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Input/AuraInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/Widget/DamageIndicatorComponent.h"

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	// only called if the PlayerController has a PlayerInput object. it will not be called in non-locally controlled PlayerController.
	Super::PlayerTick(DeltaTime);

	// Caching Target HitResult
	GetHitResultUnderCursor(ECC_Target, false, TargetHitResult);
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

	/* Input */
	check(AuraContext);
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	SetInGameInputMode();
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	/* Bind Action */
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAbilityActions(AuraInputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetAuraAbilitySystemComponent())
	{
		AuraAbilitySystemComponent->AbilityInputTagPressed(InputTag);
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetAuraAbilitySystemComponent())
	{
		AuraAbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetAuraAbilitySystemComponent())
	{
		AuraAbilitySystemComponent->AbilityInputTagHeld(InputTag);
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
