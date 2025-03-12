// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AuraGameInstance.h"

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"	// Needed
#include "Blueprint/UserWidget.h"
#include "GameUserSettings/AuraGameUserSettings.h"

void UAuraGameInstance::Init()
{
	Super::Init();

	const IOnlineSessionPtr SessionInterface = UMultiplayerSessionsSubsystem::GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		// 게임 초대를 수락하면 OnSessionUserInviteAccepted 호출하도록 연동
		OnSessionUserInviteAcceptedDelegateHandle = SessionInterface->AddOnSessionUserInviteAcceptedDelegate_Handle(FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &ThisClass::OnSessionUserInviteAccepted));
	}

	if (UAuraGameUserSettings* AuraGameUserSettings = Cast<UAuraGameUserSettings>(GEngine ? GEngine->GetGameUserSettings() : nullptr))
	{
		// GameUserSettings 로드 및 적용 
		AuraGameUserSettings->InitializeGameUserSettings(GetWorld());
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

void UAuraGameInstance::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult)
{
	if (!bWasSuccessful)
	{
		return;
	}

	// Loading Overlay 표시
	DisplayLoadingOverlay(true);
		
	if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GetSubsystem<UMultiplayerSessionsSubsystem>())
	{
		// JoinSession이 성공하지 못했으면 Loading Overlay 숨김
		OnJoinSessionCompleteDelegateHandle = MultiplayerSessionsSubsystem->AuraOnJoinSessionCompleteDelegate.AddLambda([this](EOnJoinSessionCompleteResult::Type Result)
		{
			if (Result != EOnJoinSessionCompleteResult::Success)
			{
				DisplayLoadingOverlay(false);
			}
			if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GetSubsystem<UMultiplayerSessionsSubsystem>())
			{
				MultiplayerSessionsSubsystem->AuraOnJoinSessionCompleteDelegate.Remove(OnJoinSessionCompleteDelegateHandle);
			}
		});
		
		// 세션 제거 후 초대한 유저의 세션에 참가
		OnDestroySessionCompleteDelegateHandle = MultiplayerSessionsSubsystem->AuraOnDestroySessionCompleteDelegate.AddLambda([this, InviteResult](bool bWasSuccessful)
		{
			if (bWasSuccessful)
			{
				if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GetSubsystem<UMultiplayerSessionsSubsystem>())
				{
					ReturnToMainMenu();
					MultiplayerSessionsSubsystem->JoinSession(InviteResult);
					MultiplayerSessionsSubsystem->AuraOnDestroySessionCompleteDelegate.Remove(OnDestroySessionCompleteDelegateHandle);
				}
			}
		});

		// Binding이 끝난 후 세션 제거 수행
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void UAuraGameInstance::DisplayLoadingOverlay(bool bDisplay)
{
	if (bDisplay)
	{
		if (!LoadingOverlay && LoadingOverlayClass)
		{
			LoadingOverlay = CreateWidget(this, LoadingOverlayClass, TEXT("LoadingScreenWidget"));
		}
		if (LoadingOverlay)
		{
			if (UGameViewportClient* Viewport = GetGameViewportClient())
			{
				Viewport->AddViewportWidgetContent(LoadingOverlay->TakeWidget(), 1000);
			}
		}
	}
	else
	{
		if (LoadingOverlay)
		{
			if (UGameViewportClient* Viewport = GetGameViewportClient())
			{
				Viewport->RemoveViewportWidgetContent(LoadingOverlay->TakeWidget());
			}
			LoadingOverlay = nullptr;
		}
	}
}
