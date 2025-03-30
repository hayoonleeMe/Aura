// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AuraUserWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Game/AuraGameStateBase.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

void UAuraUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (bUseUIMapping)
	{
		AddUIMappingContext();
	}
}

void UAuraUserWidget::NativeDestruct()
{
	OnRemovedDelegate.Broadcast();
	if (bUseUIMapping)
	{
		RemoveUIMappingContext();
	}
	Super::NativeDestruct();
}

void UAuraUserWidget::AddUIMappingContext()
{
	if (AAuraPlayerController* AuraPC = GetOwningPlayer<AAuraPlayerController>())
	{
		AuraPC->OnCloseUIActionStartedDelegate.AddUObject(this, &ThisClass::RemoveFromParent);
		AuraPC->AddUIMappingContext();
	}
}

void UAuraUserWidget::RemoveUIMappingContext()
{
	if (AAuraPlayerController* AuraPC = GetOwningPlayer<AAuraPlayerController>())
	{
		AuraPC->OnCloseUIActionStartedDelegate.RemoveAll(this);
		AuraPC->RemoveUIMappingContext();
	}
}
