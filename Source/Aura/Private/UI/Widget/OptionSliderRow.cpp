// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/OptionSliderRow.h"

#include "Components/Slider.h"
#include "Components/TextBlock.h"

void UOptionSliderRow::NativeConstruct()
{
	Super::NativeConstruct();

	Slider->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderValueChanged);
}

void UOptionSliderRow::InitializeSliderValue(int32 Value) const
{
	Slider->SetValue(Value);
	Text_Value->SetText(FText::AsNumber(Value));
}

void UOptionSliderRow::OnSliderValueChanged(float Value)
{
	Text_Value->SetText(FText::AsNumber(int32(Value)));
}
