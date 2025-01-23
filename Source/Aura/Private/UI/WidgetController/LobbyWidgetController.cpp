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
}
