// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/ToolTip_CurrentMaxValue.h"

#include "Components/TextBlock.h"

UToolTip_CurrentMaxValue::UToolTip_CurrentMaxValue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NumberFormattingOptions.MinimumFractionalDigits = 1;
	NumberFormattingOptions.MaximumFractionalDigits = 1;
}

void UToolTip_CurrentMaxValue::UpdateValues(float CurrentValue, float MaxValue) const
{
	TextBlock_CurrentValue->SetText(FText::AsNumber(CurrentValue, &NumberFormattingOptions));
	TextBlock_MaxValue->SetText(FText::AsNumber(MaxValue, &NumberFormattingOptions));
}
