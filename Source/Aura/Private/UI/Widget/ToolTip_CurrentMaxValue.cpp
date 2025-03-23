// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/ToolTip_CurrentMaxValue.h"

#include "Components/TextBlock.h"

void UToolTip_CurrentMaxValue::UpdateValues(float CurrentValue, float MaxValue) const
{
	TextBlock_CurrentValue->SetText(FText::AsNumber(CurrentValue));
	TextBlock_MaxValue->SetText(FText::AsNumber(MaxValue));
}
