// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/OptionButtonRow.h"

#include "Components/Button.h"
#include "UI/Widget/SquareButton.h"

void UOptionButtonRow::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Option->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnButtonClicked);
}

void UOptionButtonRow::OnButtonClicked()
{
}
