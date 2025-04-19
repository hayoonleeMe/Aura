// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LobbyOverlay.h"

#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Game/AuraGameInstance.h"
#include "UI/Widget/FriendsListRow.h"
#include "UI/Widget/SquareButton.h"

ULobbyOverlay::ULobbyOverlay(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FriendsListRefreshIntervalSeconds = 5.f;
	EmptyPlayerName = TEXT("Empty");
}

void ULobbyOverlay::CloseMenu()
{
	Super::CloseMenu();
	
	if (MultiplayerSessionsSubsystem)
	{
		// DestroySession이 끝날 때 호출되는 콜백 함수 연동 
		MultiplayerSessionsSubsystem->AuraOnDestroySessionCompleteDelegate.AddUObject(this, &ThisClass::OnDestroySessionComplete);
		
		MultiplayerSessionsSubsystem->DestroySession();
		Button_Exit->InternalButton->SetIsEnabled(false);
	}
}

void ULobbyOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	check(FriendsListRowClass);

	MultiplayerSessionsSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>() : nullptr;
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->InvitableFriendsListDelegate.AddUObject(this, &ThisClass::InvitableFriendsListArrived);
	}

	Button_Start->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnStartButtonClicked);
	Button_Exit->InternalButton->OnClicked.AddDynamic(this, &ThisClass::CloseMenu);
	if (GetOwningPlayer() && !GetOwningPlayer()->HasAuthority())
	{
		// Start 버튼은 서버에서만 표시
		Button_Start->RemoveFromParent();	
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(RefreshFriendsListTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::RequestInvitableFriendsList), FriendsListRefreshIntervalSeconds, true, 0.f);
	}
}

void ULobbyOverlay::NativeDestruct()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(RefreshFriendsListTimerHandle);
	}

	// 로비에서 게임 시작 시 MainMenuMusic 중단
	if (!bExit)
	{
		if (UAuraGameInstance* AuraGameInstance = GetGameInstance<UAuraGameInstance>())
		{
			AuraGameInstance->StopMainMenuMusic();
		}
	}
	
	Super::NativeDestruct();
}

void ULobbyOverlay::OnStartButtonClicked()
{
	const APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController && PlayerController->HasAuthority() && GetWorld())
	{
		GetWorld()->ServerTravel(TEXT("/Game/Maps/StartupMap?listen"));
	}
}

void ULobbyOverlay::OnDestroySessionComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		bExit = true;
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			GameInstance->ReturnToMainMenu();
		}
	}
	else
	{
		// 실패하면 재시도
		if (MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->DestroySession();
		}
	}
}

void ULobbyOverlay::RequestInvitableFriendsList() const
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->RequestInvitableFriendsList();
	}
}

void ULobbyOverlay::InvitableFriendsListArrived(bool bWasSuccessful, TArray<TSharedRef<FOnlineFriend>> FriendsList)
{
	if (!bWasSuccessful)
	{
		return;
	}

	ScrollBox_FriendsList->ClearChildren();

	for (const TSharedRef<FOnlineFriend>& Friend : FriendsList)
	{
		if (UFriendsListRow* Row = CreateWidget<UFriendsListRow>(this, FriendsListRowClass))
		{
			Row->PlayerName = Friend->GetDisplayName();
			Row->UserId = Friend->GetUserId();
			Row->Button_Invite->SetIsEnabled(bCanInvite);
			
			ScrollBox_FriendsList->AddChild(Row);
		}
	}
}

void ULobbyOverlay::OnNewPlayerAdded(const FString& PlayerName)
{
	++NumLobbyPlayers;

	if (NumLobbyPlayers == 1)
	{
		Text_HostPlayerName->SetText(FText::FromString(PlayerName));
	}
	else if (NumLobbyPlayers == 2)
	{
		Text_ClientPlayerName->SetText(FText::FromString(PlayerName));
		EnableInviteButton(false);
	}
}

void ULobbyOverlay::OnPlayerLeft()
{
	--NumLobbyPlayers;
	if (NumLobbyPlayers == 1)
	{
		Text_ClientPlayerName->SetText(FText::FromString(EmptyPlayerName));
		EnableInviteButton(true);
	}
}

void ULobbyOverlay::EnableInviteButton(bool bInCanInvite)
{
	bCanInvite = bInCanInvite;

	for (UWidget* Child : ScrollBox_FriendsList->GetAllChildren())
	{
		if (const UFriendsListRow* Row = Cast<UFriendsListRow>(Child))
		{
			Row->Button_Invite->SetIsEnabled(bCanInvite);
		}
	}
}
