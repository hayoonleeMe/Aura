// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/OptionCheckBoxRow.h"

#include "Components/CheckBox.h"

void UOptionCheckBoxRow::NativeConstruct()
{
	Super::NativeConstruct();

	CheckBox->SetCheckedState(bChecked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
}
