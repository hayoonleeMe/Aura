// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/MenuShortcutAlert.h"

#include "Components/TextBlock.h"

void UMenuShortcutAlert::SetText(EGameMenuType GameMenuType, const FKey& Key) const
{
	TextBlock_Menu->SetText(FText::FromString(LexToString(GameMenuType)));
	TextBlock_Key->SetText(FText::FromString(Key.ToString()));
}
