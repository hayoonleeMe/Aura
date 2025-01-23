// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"

#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Online/OnlineSessionNames.h"

// Custom Log
DEFINE_LOG_CATEGORY(LogMultiplayerSessions);

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
	OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete_Internal)),
	OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionComplete_Internal)),
	OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnJoinSessionComplete_Internal)),
	OnDestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnDestroySessionComplete_Internal))
{
}

void UMultiplayerSessionsSubsystem::CreateSession()
{
	LOG_DARK_YELLOW(TEXT("%hs Called"), __FUNCTION__);
	
	const IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		AuraOnCreateSessionCompleteDelegate.Broadcast(false);
		return;
	}

	// 이미 세션이 존재하면 세션 제거. 제거가 완료되면 다시 CreateSession() 호출됨
	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		UE_LOG(LogMultiplayerSessions, Warning, TEXT("%hs Called, Session already exists"), __FUNCTION__);
		bCreateSessionOnDestroy = true;
		DestroySession();
		return;
	}

	// Store the delegate in a FDelegateHandle so we can later remove it from the delegate list
	OnCreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
	
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	
	// OnlineSubsystem을 사용하지 않으면 LANMatch
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == TEXT("NULL");

	// 게임에 존재할 수 있는 최대 플레이어의 수
	LastSessionSettings->NumPublicConnections = 2;	// TODO : Parameterize

	// 세션이 작동중일 때 다른 플레이어가 참가할 수 있는지 여부
	LastSessionSettings->bAllowJoinInProgress = true;						

	// Presence로 참가할 수 있는지 여부, Presence : 게임을 찾을 때 같은 지역의 플레이어만 참가할 수 있도록 하는 것
	// Steam 친구만 게임에 참가할 수 있다.
	LastSessionSettings->bAllowJoinViaPresenceFriendsOnly = true;

	// 세션에 초대할 수 있는지
	LastSessionSettings->bAllowInvites = true;
	
	// 스팀이 세션을 광고하여 다른 플레이어가 세션을 찾아서 참가할 수 있는지 여부
	LastSessionSettings->bShouldAdvertise = true;

	// 유저 Presence 정보를 표시할 것인지 여부
	LastSessionSettings->bUsesPresence = true;
	
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	
	// 1로 설정하면 여러 유저가 각각 고유의 빌드와 호스팅을 할 수 있다. 이후 유효한 게임 세션을 검색할 때 각각의 여러 세션들을 검색하고 참가할 수 있다. 만약 1이 아니면 다른 유저들의 세션들을 볼 수 없고 첫번째로 열리는 게임 세션에 참가하려고 할 것이다.
	LastSessionSettings->BuildUniqueId = 1;

	// Dev App Id로 480을 사용할 때 식별을 위한 데이터 추가 // TODO : Parameterize
	LastSessionSettings->Set(FName(TEXT("Identifier")), FString(TEXT("htdovad")), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
	{
		if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
		{
			UE_LOG(LogMultiplayerSessions, Error, TEXT("%hs Called, CreateSession failed"), __FUNCTION__);
			SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			AuraOnCreateSessionCompleteDelegate.Broadcast(false);
		}	
	}
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete_Internal(FName SessionName, bool bWasSuccessful)
{
	LOG_DARK_YELLOW(TEXT("%hs Called, SessionName %s"), __FUNCTION__, *SessionName.ToString());
	
	AuraOnCreateSessionCompleteDelegate.Broadcast(bWasSuccessful);
	
	const IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
	}

	if (bWasSuccessful)
	{
		if (UWorld* World = GetWorld())
		{
			// Non-Seamless Travel (No need to seamless travel. Set GameMode's bUseSeamlessTravel to false)
			World->ServerTravel(TEXT("/Game/Maps/Lobby?listen"));	// TODO : Parameterize
		}
	}
}

void UMultiplayerSessionsSubsystem::FindSessions()
{
	LOG_DARK_YELLOW(TEXT("%hs Called"), __FUNCTION__);
	
	const IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
    {
		AuraOnFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
        return;
    }

    OnFindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

    LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	
	// 세션 검색 결과의 최대 수, 현재 Dev App Id를 480으로 쓰므로 크게 설정한다.
    LastSessionSearch->MaxSearchResults = 10000; // TODO : Parameterize?	
	
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == TEXT("NULL");
	
	// 우리가 찾는 세션이 Presence를 사용하는 것을 명시하도록 쿼리세팅 설정
    LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);	

    if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
    {
    	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
    	{
    		UE_LOG(LogMultiplayerSessions, Error, TEXT("%hs Called, FindSession Failed"), __FUNCTION__);
    		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
    		AuraOnFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
    		AuraOnFindFirstSessionCompleteDelegate.Broadcast(FOnlineSessionSearchResult(), false);
    	}	
    }
}

void UMultiplayerSessionsSubsystem::OnFindSessionComplete_Internal(bool bWasSuccessful)
{
	LOG_DARK_YELLOW(TEXT("%hs Called, SerachResults Num : %d"), __FUNCTION__, LastSessionSearch->SearchResults.Num());
	
	const IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	}

	if (!bWasSuccessful)
	{
		AuraOnFindFirstSessionCompleteDelegate.Broadcast(FOnlineSessionSearchResult(), false);
		AuraOnFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	// 식별된 모든 결과 저장
	TArray<FOnlineSessionSearchResult> IdentifiedResults;
	
	// 첫번째 세션을 반환했는지 여부
	bool bBroadcastFirstSession = false;

	const FString IdentifierValue(TEXT("htdovad"));	// TODO : 식별자 변경?
	for (const FOnlineSessionSearchResult& Result : LastSessionSearch->SearchResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName(TEXT("Identifier")), SettingsValue);
		
		// Session 찾음
		if (SettingsValue == IdentifierValue)
		{
			if (!bBroadcastFirstSession)
			{
				// 첫번째 세션만 반환
				bBroadcastFirstSession = true;
				AuraOnFindFirstSessionCompleteDelegate.Broadcast(Result, true);
			}

			// 식별된 모든 결과 저장
			IdentifiedResults.Add(Result);
		}
	}

	// 모든 결과 반환
	AuraOnFindSessionsCompleteDelegate.Broadcast(IdentifiedResults, true);
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	LOG_DARK_YELLOW(TEXT("%hs Called"), __FUNCTION__);
	
	const IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		AuraOnJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}
	
	OnJoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		UE_LOG(LogMultiplayerSessions, Error, TEXT("%hs Called, JoinSession failed"), __FUNCTION__);
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
		AuraOnJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete_Internal(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	LOG_DARK_YELLOW(TEXT("%hs Called, SessionName %s, Result %s"), __FUNCTION__, *SessionName.ToString(), LexToString(Result));
	
	AuraOnJoinSessionCompleteDelegate.Broadcast(Result);

	const IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		return;
	}

	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString Address;
		if (SessionInterface->GetResolvedConnectString(SessionName, Address))
		{
			if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
			{
				PlayerController->ClientTravel(Address, TRAVEL_Absolute);
			}
		}
	}
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	LOG_DARK_YELLOW(TEXT("%hs Called"), __FUNCTION__);
	
	const IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		AuraOnDestroySessionCompleteDelegate.Broadcast(false);
		return;
	}

	OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

	if (!GetNamedOnlineSession())
	{
		// 제거할 세션이 없으면 true 반환하고 끝냄
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		AuraOnDestroySessionCompleteDelegate.Broadcast(true);
	}
	else if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		UE_LOG(LogMultiplayerSessions, Error, TEXT("%hs Called, DestroySession failed"), __FUNCTION__);
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		AuraOnDestroySessionCompleteDelegate.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete_Internal(FName SessionName, bool bWasSuccessful)
{
	LOG_DARK_YELLOW(TEXT("%hs Called, SessionName %s"), __FUNCTION__, *SessionName.ToString());
	
	AuraOnDestroySessionCompleteDelegate.Broadcast(bWasSuccessful);
	
	const IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
	}

	// 세션을 성공적으로 삭제했고, 세션 생성 중에 지운 경우라면 다시 세션 생성
	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		CreateSession();
	}
}

FNamedOnlineSession* UMultiplayerSessionsSubsystem::GetNamedOnlineSession(FName SessionName)
{
	const IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		return SessionInterface->GetNamedSession(SessionName);
	}
	return nullptr;
}

FString UMultiplayerSessionsSubsystem::GetDebugSessionInfo(FName SessionName)
{
	const IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		return FString(TEXT("Invalid SessionInterface"));
	}

	if (FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(SessionName))
	{
		FString Str;
		Str += FString::Printf(TEXT("Valid NamedOnlineSession, [SessionName %s]  "), *NamedSession->SessionName.ToString());

		Str += TEXT("[SessionId ");
		if (NamedSession->SessionInfo.IsValid() && NamedSession->SessionInfo->GetSessionId().IsValid())
		{
			Str += NamedSession->SessionInfo->GetSessionId().ToString();
		}
		else
		{
			Str += TEXT("Invalid");
		}
		Str += TEXT("]  ");

		Str += TEXT("[OwningUserId ");
		if (NamedSession->OwningUserId.IsValid())
		{
			Str += NamedSession->OwningUserId->ToString();
		}
		else
		{
			Str += TEXT("Invalid");
		}
		Str += TEXT("]  ");

		Str += TEXT("[LocalOwnerId ");
		if (NamedSession->LocalOwnerId.IsValid())
		{
			Str += NamedSession->LocalOwnerId->ToString();
		}
		else
		{
			Str += TEXT("Invalid");
		}
		Str += TEXT("]  ");
		
		Str += FString::Printf(TEXT("RegisteredPlayers : [RegisteredPlayers.Num() %d, "), NamedSession->RegisteredPlayers.Num());
		for (int32 Index = 0; Index < NamedSession->RegisteredPlayers.Num(); ++Index)
		{
			Str += FString::Printf(TEXT("%d : "), Index);
			const FUniqueNetIdRef IdRef = NamedSession->RegisteredPlayers[Index];
			
			if (IdRef->IsValid())
			{
				Str += IdRef->ToString() + TEXT(", ");
			}
			else
			{
				Str += TEXT("Invalid");
			}
		}
		Str += TEXT("]  ");

		return Str;
	}
	return FString(TEXT("Invalid NamedOnlineSession"));
}

IOnlineSessionPtr UMultiplayerSessionsSubsystem::GetSessionInterface()
{
	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem && OnlineSubsystem->GetSessionInterface())
	{
		return OnlineSubsystem->GetSessionInterface();
	}
	UE_LOG(LogMultiplayerSessions, Error, TEXT("Invalid Session Interface"));
	return nullptr;
}

IOnlineFriendsPtr UMultiplayerSessionsSubsystem::GetFriendsInterface()
{
	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem && OnlineSubsystem->GetFriendsInterface())
	{
		return OnlineSubsystem->GetFriendsInterface();
	}
	UE_LOG(LogMultiplayerSessions, Error, TEXT("Invalid Friends Interface"));
	return nullptr;
}

void UMultiplayerSessionsSubsystem::RequestInvitableFriendsList()
{
	LOG_DARK_YELLOW(TEXT("%hs Called"), __FUNCTION__);
	
	const IOnlineFriendsPtr FriendsInterface = GetFriendsInterface();
	if (FriendsInterface.IsValid())
	{
		const FOnReadFriendsListComplete Delegate = FOnReadFriendsListComplete::CreateUObject(this, &ThisClass::OnReadFriendsComplete);
		FriendsInterface->ReadFriendsList(0, EFriendsLists::ToString(EFriendsLists::Default), Delegate);
	}
}

void UMultiplayerSessionsSubsystem::SendSessionInvite(const FUniqueNetId& FriendId)
{
	LOG_DARK_YELLOW(TEXT("%hs Called"), __FUNCTION__);
	
	const IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->SendSessionInviteToFriend(0, NAME_GameSession, FriendId);
	}
}

void UMultiplayerSessionsSubsystem::PrintFriendDebugLog(const FOnlineFriend& Friend)
{
	const FOnlineUserPresence& Presence = Friend.GetPresence();
	UE_LOG(LogMultiplayerSessions, Log, TEXT("\t%s has unique id (%s)"), *Friend.GetDisplayName(), *Friend.GetUserId()->ToDebugString());
	UE_LOG(LogMultiplayerSessions, Log, TEXT("\t\t Invite status (%s)"), EInviteStatus::ToString(Friend.GetInviteStatus()));
	UE_LOG(LogMultiplayerSessions, Log, TEXT("\t\t Presence: %s"), *Presence.Status.StatusStr);
	UE_LOG(LogMultiplayerSessions, Log, TEXT("\t\t State: %s"), EOnlinePresenceState::ToString(Presence.Status.State));
	UE_LOG(LogMultiplayerSessions, Log, TEXT("\t\t bIsOnline (%s)"), Presence.bIsOnline ? TEXT("true") : TEXT("false"));
	UE_LOG(LogMultiplayerSessions, Log, TEXT("\t\t bIsPlaying (%s)"), Presence.bIsPlaying ? TEXT("true") : TEXT("false"));
	UE_LOG(LogMultiplayerSessions, Log, TEXT("\t\t bIsPlayingThisGame (%s)"), Presence.bIsPlayingThisGame ? TEXT("true") : TEXT("false"));
	UE_LOG(LogMultiplayerSessions, Log, TEXT("\t\t bIsJoinable (%s)"), Presence.bIsJoinable ? TEXT("true") : TEXT("false"));
}

void UMultiplayerSessionsSubsystem::OnReadFriendsComplete(int32 LocalPlayer, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr) const
{
	LOG_DARK_YELLOW(TEXT("%hs Called"), __FUNCTION__);
	
	if (!bWasSuccessful)
	{
		UE_LOG(LogMultiplayerSessions, Error, TEXT("%hs Called, ReadFriends Fail, ErrorStr %s"), __FUNCTION__, *ErrorStr);
		InvitableFriendsListDelegate.Broadcast(false, TArray<TSharedRef<FOnlineFriend>>());
		return;
	}

	const IOnlineFriendsPtr FriendsInterface = GetFriendsInterface();
	if (FriendsInterface.IsValid())
	{
		TArray<TSharedRef<FOnlineFriend>> Friends;
		if (FriendsInterface->GetFriendsList(LocalPlayer, ListName, Friends))
		{
			// 초대할 수 있는 친구 목록을 반환
			TArray<TSharedRef<FOnlineFriend>> InvitableFriendsList;
			for (const TSharedRef<FOnlineFriend>& FriendSharedRef : Friends)
			{
				const FOnlineFriend& Friend = *FriendSharedRef;
				const FOnlineUserPresence& Presence = Friend.GetPresence();
				if (Presence.bIsOnline && Presence.bIsPlayingThisGame)
				{
					InvitableFriendsList.Add(FriendSharedRef);
				}
			}

			InvitableFriendsListDelegate.Broadcast(true, InvitableFriendsList);
			return;
		}
	}

	InvitableFriendsListDelegate.Broadcast(false, TArray<TSharedRef<FOnlineFriend>>());
}
