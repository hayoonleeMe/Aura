// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/PlayerNameplate.h"

#include "Components/TextBlock.h"

void UPlayerNameplate::SetPlayerName(const FString& PlayerName) const
{
	Text_PlayerName->SetText(FText::FromString(PlayerName));
}
