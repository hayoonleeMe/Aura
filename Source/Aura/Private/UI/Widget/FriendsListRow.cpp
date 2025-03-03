// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/FriendsListRow.h"

#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UI/Widget/SquareButton.h"

void UFriendsListRow::NativeConstruct()
{
	Super::NativeConstruct();

	Text_Name->SetText(FText::FromString(PlayerName));

	Button_Invite->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnInviteButtonClicked);
}

void UFriendsListRow::OnInviteButtonClicked()
{
	if (UserId->IsValid())
	{
		UMultiplayerSessionsSubsystem::SendSessionInvite(*UserId);
	}
}
