// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/GameOverlay.h"

void AAuraHUD::InitOverlay()
{
	check(GameOverlayClass);

	GameOverlay = CreateWidget<UGameOverlay>(GetOwningPlayerController(), GameOverlayClass);
	if (GameOverlay)
	{
		// 화면에 표시
		GameOverlay->AddToViewport();
	}
}

void AAuraHUD::OpenMenu(EGameMenuType GameMenuType) const
{
	if (GameOverlay)
	{
		if (GameMenuType == EGameMenuType::AttributeMenu)
		{
			GameOverlay->OpenAttributeMenu();	
		}
		else if (GameMenuType == EGameMenuType::SpellMenu)
		{
			GameOverlay->OpenSpellMenu();
		}
		else if (GameMenuType == EGameMenuType::PauseMenu)
		{
			GameOverlay->OpenPauseMenu();
		}
		else if (GameMenuType == EGameMenuType::TutorialMenu)
		{
			GameOverlay->OpenTutorialMenu();
		}
	}
}

void AAuraHUD::ShowGameOverlay(bool bShow) const
{
	if (GameOverlay)
	{
		GameOverlay->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void AAuraHUD::CloseAllMenu() const
{
	if (GameOverlay)
	{
		GameOverlay->CloseAllMenu();
	}
}

void AAuraHUD::ShowStageReadyHUD() const
{
	if (GameOverlay)
	{
		GameOverlay->ShowStageReadyHUD();
	}
}

void AAuraHUD::ShowAllMenuShortcutAlert(const TArray<TTuple<EGameMenuType, FKey>>& MenuKeys) const
{
	if (GameOverlay)
	{
		GameOverlay->ShowAllMenuShortcutAlert(MenuKeys);
	}
}
