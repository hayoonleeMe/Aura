// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AuraUserWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Game/AuraGameStateBase.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

UAuraAbilitySystemComponent* UAuraUserWidget::GetOwnerAuraAbilitySystemComponentChecked() const
{
	UAbilitySystemComponent* ASC = GetOwningPlayer() ? UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayer()->GetPawn()) : nullptr;
	return CastChecked<UAuraAbilitySystemComponent>(ASC);
}

UAuraAttributeSet* UAuraUserWidget::GetOwnerAuraAttributeSetChecked() const
{
	return CastChecked<UAuraAttributeSet>(GetOwningPlayerState<AAuraPlayerState>(true)->GetAttributeSet());
}

AAuraGameStateBase* UAuraUserWidget::GetAuraGameStateBaseChecked() const
{
	AGameStateBase* GameStateBase = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	return CastChecked<AAuraGameStateBase>(GameStateBase);
}

void UAuraUserWidget::SetInGameInputMode() const
{
	if (AAuraPlayerController* AuraPC = GetOwningPlayer<AAuraPlayerController>())
	{
		AuraPC->SetInGameInputMode();
	}
}

void UAuraUserWidget::SetUIInputMode(UUserWidget* WidgetToFocus) const
{
	if (AAuraPlayerController* AuraPC = GetOwningPlayer<AAuraPlayerController>())
	{
		AuraPC->SetUIInputMode(WidgetToFocus);
	}
}

void UAuraUserWidget::NativeDestruct()
{
	OnRemovedDelegate.Broadcast();
	Super::NativeDestruct();
}
