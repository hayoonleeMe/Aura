// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LobbyGameStateBase.h"

#include "GameFramework/PlayerState.h"
#include "UI/HUD/LobbyHUD.h"
#include "UI/WidgetController/LobbyWidgetController.h"

// Called on server and client
void ALobbyGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	// 다음 프레임에 새롭게 들어온 플레이어를 Lobby Widget에 업데이트
	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this, PlayerState]()
	{
		if (const APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
		{
			if (const ALobbyHUD* LobbyHUD = PC->GetHUD<ALobbyHUD>())
			{
				LobbyHUD->GetLobbyWidgetController()->OnNewPlayerAdded(PlayerState->GetPlayerName());
			}
		}
	}));
}

void ALobbyGameStateBase::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	// 나가는 플레이어를 Lobby Widget에 업데이트
	if (const APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
	{
		if (const ALobbyHUD* LobbyHUD = PC->GetHUD<ALobbyHUD>())
		{
			LobbyHUD->GetLobbyWidgetController()->OnPlayerLeft();
		}
	}
}
