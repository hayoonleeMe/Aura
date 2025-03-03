// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/ToolTip_AttributeRow.h"

#include "Components/TextBlock.h"

void UToolTip_AttributeRow::NativeConstruct()
{
	Super::NativeConstruct();
	
	Text_ToolTip->SetText(Description);
}
