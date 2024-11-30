// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAS = GetCheckedAuraAttributeSet();
	
	OnHealthChangedDelegate.Broadcast(AuraAS->GetHealth());
	OnMaxHealthChangedDelegate.Broadcast(AuraAS->GetMaxHealth());
	OnManaChangedDelegate.Broadcast(AuraAS->GetMana());
	OnMaxManaChangedDelegate.Broadcast(AuraAS->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAS = GetCheckedAuraAttributeSet();
	UAuraAbilitySystemComponent* AuraASC = GetCheckedAuraAbilitySystemComponent();

	// Attribute가 변경되면 Widget으로 새 데이터 전송
	AuraASC->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		OnHealthChangedDelegate.Broadcast(Data.NewValue);
	});
	AuraASC->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		OnMaxHealthChangedDelegate.Broadcast(Data.NewValue);
	});
	AuraASC->GetGameplayAttributeValueChangeDelegate(AuraAS->GetManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		OnManaChangedDelegate.Broadcast(Data.NewValue);
	});
	AuraASC->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		OnMaxManaChangedDelegate.Broadcast(Data.NewValue);
	});
}