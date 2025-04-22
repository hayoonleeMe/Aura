// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/HealthGlobe.h"

#include "AuraBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/Widget/ToolTip_CurrentMaxValue.h"
#include "GameFramework/PlayerState.h"

UHealthGlobe::UHealthGlobe(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ToolTipWidgetDelegate.BindDynamic(this, &ThisClass::GetTooltipWidget);
}

void UHealthGlobe::NativeConstruct()
{
	Super::NativeConstruct();

	check(ToolTipWidgetClass);

	UAuraAbilitySystemComponent* AuraASC = UAuraBlueprintLibrary::GetAuraAbilitySystemComponentChecked(GetOwningPlayerState());
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetHealthAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateHealth(Data.NewValue);
	});
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetMaxHealthAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateMaxHealth(Data.NewValue);
	});

	const UAuraAttributeSet* AuraAS = UAuraBlueprintLibrary::GetAuraAttributeSetChecked(GetOwningPlayerState());
	UpdateHealth(AuraAS->GetHealth());
	UpdateMaxHealth(AuraAS->GetMaxHealth());
}

void UHealthGlobe::UpdateHealth(float NewValue)
{
	Health = NewValue;
	TargetPercent = UKismetMathLibrary::SafeDivide(Health, MaxHealth);
	UpdateToolTipWidget();
}

void UHealthGlobe::UpdateMaxHealth(float NewValue)
{
	MaxHealth = NewValue;
	TargetPercent = UKismetMathLibrary::SafeDivide(Health, MaxHealth);
	UpdateToolTipWidget();
}

UWidget* UHealthGlobe::GetTooltipWidget()
{
	if (!CachedToolTipWidget)
	{
		CachedToolTipWidget = CreateWidget<UToolTip_CurrentMaxValue>(this, ToolTipWidgetClass);
	}
	if (CachedToolTipWidget)
	{
		CachedToolTipWidget->UpdateValues(Health, MaxHealth);
	}
	return CachedToolTipWidget;
}

void UHealthGlobe::UpdateToolTipWidget() const
{
	if (CachedToolTipWidget)
	{
		CachedToolTipWidget->UpdateValues(Health, MaxHealth);
	}
}
