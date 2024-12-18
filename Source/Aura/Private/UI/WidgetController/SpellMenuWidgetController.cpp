// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	// SpellPoints 값 전달
	const AAuraPlayerState* AuraPS = GetAuraPlayerStateChecked();
	OnSpellPointsChangedDelegate.Broadcast(AuraPS->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	// SpellPoints가 변경되면 그 값을 전달
	AAuraPlayerState* AuraPS = GetAuraPlayerStateChecked();
	AuraPS->OnSpellPointsChangedDelegate.AddWeakLambda(this, [this](int32 Value)
	{
		OnSpellPointsChangedDelegate.Broadcast(Value);
	});
}
