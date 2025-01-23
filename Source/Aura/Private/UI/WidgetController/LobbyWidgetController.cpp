// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/LobbyWidgetController.h"

#include "MultiplayerSessionsSubsystem.h"

void ULobbyWidgetController::BroadcastInitialValues()
{
	RequestInvitableFriendsList();
}

void ULobbyWidgetController::BindCallbacksToDependencies()
{
	// Caching MultiplayerSessionsSubsystem
	if (GetWorld())
	{
		if (const UGameInstance* GameInstance = GetWorld()->GetGameInstance())
		{
			MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		}
	}

	if (MultiplayerSessionsSubsystem)
	{
		// 초대할 수 있는 친구 목록을 요청할 때의 결과를 받을 콜백 함수를 연동
		MultiplayerSessionsSubsystem->InvitableFriendsListDelegate.AddUObject(this, &ThisClass::InvitableFriendsListArrived);

		// DestroySession이 종료되면 호출할 콜백 함수 등록
		MultiplayerSessionsSubsystem->AuraOnDestroySessionCompleteDelegate.AddUObject(this, &ThisClass::OnDestroySessionComplete);
	}
}

void ULobbyWidgetController::RequestInvitableFriendsList()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->RequestInvitableFriendsList();
	}
}

void ULobbyWidgetController::InvitableFriendsListArrived(bool bWasSuccessful, TArray<TSharedRef<FOnlineFriend>> FriendsList)
{
	if (!bWasSuccessful)
	{
		return;
	}
	
	CachedFriendsList = MoveTemp(FriendsList);
	for (int32 Index = 0; Index < CachedFriendsList.Num(); ++Index)
	{
		const FOnlineFriend& Friend = *CachedFriendsList[Index];
		OnInvitableFriendDelegate.Broadcast(Index, *Friend.GetDisplayName());
	}
}

void ULobbyWidgetController::ReturnToMainMenu()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void ULobbyWidgetController::StartGame(const APlayerController* OwningPlayerController)
{
	if (IsValid(OwningPlayerController) && OwningPlayerController->HasAuthority())
	{
		GetWorld()->ServerTravel(TEXT("/Game/Maps/StartupMap?listen"));
	}
}

void ULobbyWidgetController::OnDestroySessionComplete(bool bWasSuccessful) const
{
	if (bWasSuccessful)
	{
		if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
		{
			GameInstance->ReturnToMainMenu();
		}
	}
}
