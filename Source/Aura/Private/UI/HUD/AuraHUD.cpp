// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h"

void AAuraHUD::InitOverlay()
{
	check(OverlayWidgetClass);

	OverlayWidget = CreateWidget<UAuraUserWidget>(GetOwningPlayerController(), OverlayWidgetClass);
	if (OverlayWidget)
	{
		// 화면에 표시
		OverlayWidget->AddToViewport();
	}
}
