// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AuraWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::ShowAttributeMenu(bool bVisible)
{
	AAuraPlayerController* AuraPC = GetAuraPlayerControllerChecked();
	
	if (const AAuraHUD* AuraHUD = AuraPC->GetHUD<AAuraHUD>())
    {
    	AuraHUD->SetAttributeMenuVisible(bVisible);
    }

    if (bVisible)
    {
		// Attribute Menu가 열리면 Game Input 방지
    	AuraPC->SetUIInputMode();
    }
    else
    {
    	AuraPC->SetInGameInputMode();
    }
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
