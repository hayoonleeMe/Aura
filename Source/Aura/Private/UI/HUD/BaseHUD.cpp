// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/BaseHUD.h"

#include "UI/Widget/OptionsMenu.h"

void ABaseHUD::AddOptionsMenu() const
{
	if (UOptionsMenu* OptionsMenu = CreateWidget<UOptionsMenu>(GetOwningPlayerController(), OptionsMenuWidgetClass, TEXT("OptionsMenu")))
	{
		OptionsMenu->SetAnchorsInViewport(FAnchors(0.f, 0.f, 1.f, 1.f));
		OptionsMenu->AddToViewport(2);	// 최상단
	}
}

void ABaseHUD::BeginPlay()
{
	Super::BeginPlay();

	check(OptionsMenuWidgetClass);
}
