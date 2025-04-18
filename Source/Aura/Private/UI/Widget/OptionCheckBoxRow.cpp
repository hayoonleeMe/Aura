// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/OptionCheckBoxRow.h"

#include "Components/CheckBox.h"

void UOptionCheckBoxRow::SetIsChecked(bool bChecked) const
{
	CheckBox->SetIsChecked(bChecked);
	CheckBox->OnCheckStateChanged.Broadcast(bChecked);
}
