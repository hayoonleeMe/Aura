// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

// WBP_Overlay의 Menu Widget Switcher를 표시하거나 숨기는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShowMenuWidgetSwitcherSignature, bool, bVisible);

/**
 * Overlay Widget에서 사용할 WidgetController
 */
UCLASS()
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnAttributeChangedSignature OnHealthChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnAttributeChangedSignature OnMaxHealthChangedDelegate;
	
	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnAttributeChangedSignature OnManaChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnAttributeChangedSignature OnMaxManaChangedDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Aura|Overlay")
	FShowMenuWidgetSwitcherSignature ShowMenuWidgetSwitcherDelegate;
};
