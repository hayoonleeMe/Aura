// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AuraGameInstance.h"

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"

void UAuraGameInstance::Init()
{
	Super::Init();

	const IOnlineSessionPtr SessionInterface = UMultiplayerSessionsSubsystem::GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		// 게임 초대를 수락하면 OnSessionUserInviteAccepted 호출하도록 연동
		OnSessionUserInviteAcceptedDelegateHandle = SessionInterface->AddOnSessionUserInviteAcceptedDelegate_Handle(FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &ThisClass::OnSessionUserInviteAccepted));
	}
}

void UAuraGameInstance::Shutdown()
{
	const IOnlineSessionPtr SessionInterface = UMultiplayerSessionsSubsystem::GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		// Clear Delegate
		SessionInterface->ClearOnSessionUserInviteAcceptedDelegate_Handle(OnSessionUserInviteAcceptedDelegateHandle);
	}
	
	Super::Shutdown();
}

void UAuraGameInstance::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult) const
{
	// 초대를 수락하면 해당 세션에 참가
	if (bWasSuccessful)
	{
		if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			MultiplayerSessionsSubsystem->JoinSession(InviteResult);
		}
	}
}
