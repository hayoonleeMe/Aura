// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/LobbyWidgetController.h"

#include "MultiplayerSessionsSubsystem.h"

void ULobbyWidgetController::BroadcastInitialValues()
{
}

void ULobbyWidgetController::BindCallbacksToDependencies()
{
	// Caching MultiplayerSessionsSubsystem
	if (GetWorld())
	{
		if (const UGameInstance* GameInstance = GetWorld()->GetGameInstance())
		{
			MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		}
	}

	if (MultiplayerSessionsSubsystem)
	{
		// DestroySession이 종료되면 호출할 콜백 함수 등록
		MultiplayerSessionsSubsystem->AuraOnDestroySessionCompleteDelegate.AddUObject(this, &ThisClass::OnDestroySessionComplete);
	}
}
void ULobbyWidgetController::ReturnToMainMenu()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void ULobbyWidgetController::StartGame(const APlayerController* OwningPlayerController)
{
	if (IsValid(OwningPlayerController) && OwningPlayerController->HasAuthority())
	{
		GetWorld()->ServerTravel(TEXT("/Game/Maps/StartupMap?listen"));
	}
}

void ULobbyWidgetController::OnDestroySessionComplete(bool bWasSuccessful) const
{
	if (bWasSuccessful)
	{
		if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
		{
			GameInstance->ReturnToMainMenu();
		}
	}
}
