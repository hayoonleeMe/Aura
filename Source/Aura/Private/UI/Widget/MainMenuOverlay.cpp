// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/MainMenuOverlay.h"

#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/Widget/SquareButton.h"

void UMainMenuOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Start->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnStartButtonClicked);
	Button_Exit->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);

	if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>() : nullptr)
	{
		MultiplayerSessionsSubsystem->AuraOnCreateSessionCompleteDelegate.AddUObject(this, &ThisClass::OnCreateSessionComplete);
	}
}

void UMainMenuOverlay::OnStartButtonClicked()
{
	if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>() : nullptr)
	{
		MultiplayerSessionsSubsystem->CreateSession();
		Button_Start->SetIsEnabled(false);
	}
}

void UMainMenuOverlay::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetOwningPlayer(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UMainMenuOverlay::OnCreateSessionComplete(bool bWasSuccessful) const
{
	if (!bWasSuccessful)
	{
		// 실패하면 다시 시도
		if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>() : nullptr)
		{
			MultiplayerSessionsSubsystem->CreateSession();
		}
	}
}
