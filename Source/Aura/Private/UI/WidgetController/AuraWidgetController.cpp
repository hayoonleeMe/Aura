// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AuraWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

AAuraPlayerController* UAuraWidgetController::GetCheckedAuraPlayerController() const
{
	return CastChecked<AAuraPlayerController>(PlayerController);
}

AAuraPlayerState* UAuraWidgetController::GetCheckedAuraPlayerState() const
{
	return CastChecked<AAuraPlayerState>(PlayerState);
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetCheckedAuraAbilitySystemComponent() const
{
	return CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
}

UAuraAttributeSet* UAuraWidgetController::GetCheckedAuraAttributeSet() const
{
	return CastChecked<UAuraAttributeSet>(AttributeSet);
}
