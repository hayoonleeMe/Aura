// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Data/AttributeConfig.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAS = GetAuraAttributeSetChecked();
	
	// 모든 Attribute에 대한 현재 값을 포함하는 AttributeInfo 전달
	for (const TTuple<FGameplayTag, FGameplayAttribute>& Tuple : AuraAS->TagToAttributeMap)
	{
		BroadcastAttributeInfo(Tuple.Key, Tuple.Value.GetNumericValue(AuraAS));
	}

	// AttributePoints 값 전달
	const AAuraPlayerState* AuraPS = GetAuraPlayerStateChecked();
	OnAttributePointsChangedDelegate.Broadcast(AuraPS->GetAttributePoints());
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

	// AttributePoints가 변경되면 그 값을 전달
	AAuraPlayerState* AuraPS = GetAuraPlayerStateChecked();
	AuraPS->OnAttributePointsChangedDelegate.AddWeakLambda(this, [this](int32 Value)
	{
		OnAttributePointsChangedDelegate.Broadcast(Value);
	});
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UAuraAbilitySystemComponent* AuraASC = GetAuraAbilitySystemComponentChecked();
	AuraASC->ServerUpgradeAttribute(AttributeTag);
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
