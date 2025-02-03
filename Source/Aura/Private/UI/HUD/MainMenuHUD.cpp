// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/MainMenuHUD.h"

#include "MultiplayerSessionsSubsystem.h"
#include "Blueprint/UserWidget.h"

void AMainMenuHUD::CreateSession() const
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			MultiplayerSessionsSubsystem->CreateSession();
		}
	}
}

void AMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	checkf(MainMenuOverlayClass, TEXT("Need to set MainMenuOverlayClass"));

	APlayerController* PC = GetOwningPlayerController();
	check(PC);

	PC->SetInputMode(FInputModeUIOnly());
	PC->SetShowMouseCursor(true);

	MainMenuOverlay = CreateWidget<UUserWidget>(GetWorld(), MainMenuOverlayClass);
	if (MainMenuOverlay)
	{
		MainMenuOverlay->AddToViewport();
	}

	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			// 세션 생성에 실패하면 Start Button 다시 활성화
			MultiplayerSessionsSubsystem->AuraOnCreateSessionCompleteDelegate.AddWeakLambda(this, [this](bool bWasSuccessful)
			{
				if (!bWasSuccessful)
				{
					EnableStartButtonDelegate.Broadcast();
				}
			});
		}
	}
}
