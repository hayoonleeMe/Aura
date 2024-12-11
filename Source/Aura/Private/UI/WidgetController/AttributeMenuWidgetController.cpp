// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Data/AttributeConfig.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAS = GetAuraAttributeSetChecked();
	
	// 모든 Attribute에 대한 현재 값을 포함하는 AttributeInfo 전달
	for (const TTuple<FGameplayTag, FGameplayAttribute>& Tuple : AuraAS->TagToAttributeMap)
	{
		BroadcastAttributeInfo(Tuple.Key, Tuple.Value.GetNumericValue(AuraAS));
	}
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAS = GetAuraAttributeSetChecked();
	
	// Attribute가 변경되면 해당 Attribute의 정보를 전달
	for (const TTuple<FGameplayTag, FGameplayAttribute>& Tuple : AuraAS->TagToAttributeMap)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Tuple.Value).AddWeakLambda(this, [this, Tuple](const FOnAttributeChangeData& AttributeChangeData)
		{
			BroadcastAttributeInfo(Tuple.Key, AttributeChangeData.NewValue);
		});
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, float AttributeValue) const
{
	if (AttributeConfig)
	{
		FAttributeInfo AttributeInfo = AttributeConfig->GetAttributeInfoByTag(AttributeTag);
		AttributeInfo.Value = AttributeValue;
		OnAttributeChangedWithInfoDelegate.Broadcast(AttributeInfo);
	}
}
