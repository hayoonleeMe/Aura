// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/MainMenuHUD.h"

#include "UI/Widget/MainMenuOverlay.h"

void AMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	checkf(MainMenuOverlayClass, TEXT("Need to set MainMenuOverlayClass"));

	APlayerController* PC = GetOwningPlayerController();
	check(PC);

	PC->SetInputMode(FInputModeUIOnly());
	PC->SetShowMouseCursor(true);

	MainMenuOverlay = CreateWidget<UMainMenuOverlay>(GetWorld(), MainMenuOverlayClass);
	if (MainMenuOverlay)
	{
		MainMenuOverlay->AddToViewport();
	}
}
