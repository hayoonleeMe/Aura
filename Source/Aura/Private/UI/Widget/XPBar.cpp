// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/XPBar.h"

#include "AuraBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/Widget/ToolTip_XPBar.h"

UXPBar::UXPBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ToolTipWidgetDelegate.BindDynamic(this, &ThisClass::GetProgressBarXPTooltipWidget);
}

void UXPBar::NativeConstruct()
{
	Super::NativeConstruct();

	check(ToolTipWidgetClass);

	UAuraAbilitySystemComponent* AuraASC = UAuraBlueprintLibrary::GetAuraAbilitySystemComponentChecked(GetOwningPlayer());
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetXPAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateXPChange(Data.NewValue);
	});
	AuraASC->GetGameplayAttributeValueChangeDelegate(UAuraAttributeSet::GetLevelAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
	{
		UpdateLevelChange(Data.NewValue);
	});

	BroadcastInitialValues();
}

void UXPBar::BroadcastInitialValues()
{
    UpdateXPChange(AuraAS->GetXP());
    UpdateLevelChange(AuraAS->GetLevel());
	const UAuraAttributeSet* AuraAS = UAuraBlueprintLibrary::GetAuraAttributeSetChecked(GetOwningPlayer());
}

UWidget* UXPBar::GetProgressBarXPTooltipWidget()
{
	UToolTip_XPBar* ToolTipUserWidget = CreateWidget<UToolTip_XPBar>(this, ToolTipWidgetClass);
	ToolTipUserWidget->Level = Level;
	ToolTipUserWidget->XP = XP;
	ToolTipUserWidget->XPRequirement = XPRequirement;
	return ToolTipUserWidget;
}

void UXPBar::UpdateXPChange(float InXP)
{
	XP = InXP;
	ProgressBar_XP->SetPercent(UKismetMathLibrary::SafeDivide(XP, XPRequirement));
}

void UXPBar::UpdateLevelChange(int32 InLevel)
{
	Level = InLevel;
	XPRequirement = UAuraBlueprintLibrary::GetLevelUpXPRequirement(GetOwningPlayer(), Level + 1);
	ProgressBar_XP->SetPercent(UKismetMathLibrary::SafeDivide(XP, XPRequirement));
}
