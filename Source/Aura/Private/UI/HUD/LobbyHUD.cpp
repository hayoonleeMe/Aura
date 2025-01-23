// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/LobbyHUD.h"

#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/LobbyWidgetController.h"

void ALobbyHUD::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(LobbyOverlayClass, TEXT("Need to set LobbyWidgetClass"));
	checkf(LobbyWidgetControllerClass, TEXT("Need to set LobbyWidgetControllerClass"));

	APlayerController* PC = GetOwningPlayerController();
	check(PC);

	PC->SetInputMode(FInputModeUIOnly());
	PC->SetShowMouseCursor(true);

	LobbyOverlay = CreateWidget<UAuraUserWidget>(GetWorld(), LobbyOverlayClass);
	if (LobbyOverlay)
	{
		LobbyOverlay->AddToViewport();
		
		LobbyWidgetController = NewObject<ULobbyWidgetController>(this, LobbyWidgetControllerClass);
		if (LobbyWidgetController)
		{
			LobbyWidgetController->BindCallbacksToDependencies();
			LobbyOverlay->SetWidgetController(LobbyWidgetController);
		}
	}
}
