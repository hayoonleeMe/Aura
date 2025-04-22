// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/XPBar.h"

#include "AuraBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/Widget/ToolTip_XPBar.h"
#include "GameFramework/PlayerState.h"

UXPBar::UXPBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ToolTipWidgetDelegate.BindDynamic(this, &ThisClass::GetProgressBarXPTooltipWidget);
}

void UXPBar::NativeConstruct()
{
	Super::NativeConstruct();

	check(ToolTipWidgetClass);

	UAuraAbilitySystemComponent* AuraASC = UAuraBlueprintLibrary::GetAuraAbilitySystemComponentChecked(GetOwningPlayerState());
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetXPAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateXPChange(Data.NewValue);
	});
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetLevelAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateLevelChange(Data.NewValue);
	});

	const UAuraAttributeSet* AuraAS = UAuraBlueprintLibrary::GetAuraAttributeSetChecked(GetOwningPlayerState());
	UpdateXPChange(AuraAS->GetXP());
	UpdateLevelChange(AuraAS->GetLevel());
}

UWidget* UXPBar::GetProgressBarXPTooltipWidget()
{
	if (!CachedToolTipWidget)
	{
		CachedToolTipWidget = CreateWidget<UToolTip_XPBar>(this, ToolTipWidgetClass);
	}
	if (CachedToolTipWidget)
	{
		CachedToolTipWidget->UpdateValues(Level, XP, XPRequirement);
	}
	return CachedToolTipWidget;
}

void UXPBar::UpdateToolTipWidget() const
{
	if (CachedToolTipWidget)
	{
		CachedToolTipWidget->UpdateValues(Level, XP, XPRequirement);
	}
}

void UXPBar::UpdateXPChange(float InXP)
{
	XP = InXP;
	ProgressBar_XP->SetPercent(UKismetMathLibrary::SafeDivide(XP, XPRequirement));
	UpdateToolTipWidget();
}

void UXPBar::UpdateLevelChange(int32 InLevel)
{
	Level = InLevel;
	XPRequirement = UAuraBlueprintLibrary::GetLevelUpXPRequirement(GetOwningPlayer(), Level + 1);
	ProgressBar_XP->SetPercent(UKismetMathLibrary::SafeDivide(XP, XPRequirement));
	UpdateToolTipWidget();
}
