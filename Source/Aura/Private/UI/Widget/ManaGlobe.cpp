// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/ManaGlobe.h"

#include "AuraBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/Widget/ToolTip_CurrentMaxValue.h"
#include "GameFramework/PlayerState.h"

UManaGlobe::UManaGlobe(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ToolTipWidgetDelegate.BindDynamic(this, &ThisClass::GetTooltipWidget);
}

void UManaGlobe::NativeConstruct()
{
	Super::NativeConstruct();

	check(ToolTipWidgetClass);

	UAuraAbilitySystemComponent* AuraASC = UAuraBlueprintLibrary::GetAuraAbilitySystemComponentChecked(GetOwningPlayerState());
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetManaAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateMana(Data.NewValue);
	});
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetMaxManaAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateMaxMana(Data.NewValue);
	});

	const UAuraAttributeSet* AuraAS = UAuraBlueprintLibrary::GetAuraAttributeSetChecked(GetOwningPlayerState());
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
