// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/ManaGlobe.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/Widget/ToolTip_CurrentMaxValue.h"

UManaGlobe::UManaGlobe(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ToolTipWidgetDelegate.BindDynamic(this, &ThisClass::GetTooltipWidget);
}

void UManaGlobe::NativeConstruct()
{
	Super::NativeConstruct();

	check(ToolTipWidgetClass);

	UAuraAbilitySystemComponent* AuraASC = GetOwnerAuraAbilitySystemComponentChecked();
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetManaAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateMana(Data.NewValue);
	});
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetMaxManaAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateMaxMana(Data.NewValue);
	});

	BroadcastInitialValues();
}

void UManaGlobe::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAS = GetOwnerAuraAttributeSetChecked();
	UpdateMana(AuraAS->GetMana());
	UpdateMaxMana(AuraAS->GetMaxMana());
}

void UManaGlobe::UpdateMana(float NewValue)
{
	Mana = NewValue;
	TargetPercent = UKismetMathLibrary::SafeDivide(Mana, MaxMana);
	UpdateToolTipWidget();
}

void UManaGlobe::UpdateMaxMana(float NewValue)
{
	MaxMana = NewValue;
	TargetPercent = UKismetMathLibrary::SafeDivide(Mana, MaxMana);
	UpdateToolTipWidget();
}

UWidget* UManaGlobe::GetTooltipWidget()
{
	if (!CachedToolTipWidget)
	{
		CachedToolTipWidget = CreateWidget<UToolTip_CurrentMaxValue>(this, ToolTipWidgetClass);
	}
	if (CachedToolTipWidget)
	{
		CachedToolTipWidget->UpdateValues(Mana, MaxMana);
	}
	return CachedToolTipWidget;
}

void UManaGlobe::UpdateToolTipWidget() const
{
	if (CachedToolTipWidget)
	{
		CachedToolTipWidget->UpdateValues(Mana, MaxMana);
	}
}
