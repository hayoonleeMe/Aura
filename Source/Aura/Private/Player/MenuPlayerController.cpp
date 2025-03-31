// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MenuPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Framework/Application/NavigationConfig.h"
#include "Input/AuraInputComponent.h"
#include "UI/HUD/BaseHUD.h"

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Disable Navigation
	FSlateApplication::Get().GetNavigationConfig()->bTabNavigation = false;
	FSlateApplication::Get().GetNavigationConfig()->bAnalogNavigation = false;
	FSlateApplication::Get().GetNavigationConfig()->bKeyNavigation = false;

	check(UIContext);
	
	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(UIContext, 0);
	}

	bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}

void AMenuPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Bind UI Context
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	if (IA_CloseUI)
	{
		AuraInputComponent->BindAction(IA_CloseUI, ETriggerEvent::Started, this, &ThisClass::OnCloseUIActionStarted);
	}
}

void AMenuPlayerController::OnCloseUIActionStarted()
{
	if (ABaseHUD* BaseHUD = GetHUD<ABaseHUD>())
	{
		BaseHUD->CloseCurrentWidget();
	}
}
