// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/ClosableWidget.h"

#include "UI/HUD/BaseHUD.h"

void UClosableWidget::CloseMenu()
{
	// BaseHUD에서 Unregister from HUD
	OnRemovedDelegate.Broadcast();
}

void UClosableWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Register to HUD
	if (ABaseHUD* BaseHUD = GetOwningPlayer() ? GetOwningPlayer()->GetHUD<ABaseHUD>() : nullptr)
	{
		BaseHUD->RegisterWidget(this);
	}
}
