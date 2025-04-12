// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/ToolTip_Beacon_StartStage.h"

#include "Components/TextBlock.h"

void UToolTip_Beacon_StartStage::SetKeyText(const FString& KeyString) const
{
	TextBlock_Key->SetText(FText::FromString(KeyString));
}
