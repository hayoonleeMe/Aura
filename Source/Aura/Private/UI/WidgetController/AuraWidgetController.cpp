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

AAuraPlayerController* UAuraWidgetController::GetAuraPlayerControllerChecked() const
{
	return CastChecked<AAuraPlayerController>(PlayerController);
}

AAuraPlayerState* UAuraWidgetController::GetAuraPlayerStateChecked() const
{
	return CastChecked<AAuraPlayerState>(PlayerState);
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraAbilitySystemComponentChecked() const
{
	return CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAttributeSetChecked() const
{
	return CastChecked<UAuraAttributeSet>(AttributeSet);
}
