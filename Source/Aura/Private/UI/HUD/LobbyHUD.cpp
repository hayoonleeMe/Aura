// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/LobbyHUD.h"

#include "Game/AuraGameInstance.h"
#include "UI/Widget/LobbyOverlay.h"

void ALobbyHUD::OnNewPlayerAdded(const FString& PlayerName) const
{
	if (LobbyOverlay)
	{
		LobbyOverlay->OnNewPlayerAdded(PlayerName);
	}
}

void ALobbyHUD::OnPlayerLeft() const
{
	if (LobbyOverlay)
	{
		LobbyOverlay->OnPlayerLeft();
	}
}

void ALobbyHUD::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(LobbyOverlayClass, TEXT("Need to set LobbyWidgetClass"));

	APlayerController* PC = GetOwningPlayerController();
	check(PC);

	PC->SetInputMode(FInputModeUIOnly());
	PC->SetShowMouseCursor(true);

	LobbyOverlay = CreateWidget<ULobbyOverlay>(GetWorld(), LobbyOverlayClass);
	if (LobbyOverlay)
	{
		LobbyOverlay->AddToViewport();
	}

	// Loading Overlay 숨김
	if (UAuraGameInstance* AuraGameInstance = GetGameInstance<UAuraGameInstance>())
	{
		AuraGameInstance->DisplayLoadingOverlay(false);
	}
}
