// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/OptionTab.h"

#include "Components/Button.h"

void UOptionTab::NativeConstruct()
{
	Super::NativeConstruct();

	InternalButton->OnClicked.AddDynamic(this, &ThisClass::Select);

	ButtonStyle = InternalButton->GetStyle();
}

void UOptionTab::Select()
{
	ButtonStyle.Normal = SelectedBrush;
	InternalButton->SetStyle(ButtonStyle);
}

void UOptionTab::Deselect()
{
	ButtonStyle.Normal = TransparentBrush;
	InternalButton->SetStyle(ButtonStyle);
}
