// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/PauseMenu.h"

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "Game/AuraGameStateBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/Widget/SquareButton.h"

void UPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	
	Button_ReturnToGame->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnToGameButtonClicked);
	Button_Options->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnOptionsButtonClicked);
	Button_LeaveGame->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnLeaveGameButtonClicked);
	Button_ExitAura->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnExitAuraButtonClicked);

	// PauseMenu Level Sequence 재생
	const AAuraGameStateBase* AuraGameStateBase = GetAuraGameStateBaseChecked();
	if (AuraGameStateBase->PauseMenuLevelSequenceActor && AuraGameStateBase->PauseMenuLevelSequenceActor->SequencePlayer)
	{
		AuraGameStateBase->PauseMenuLevelSequenceActor->SequencePlayer->Play();
	}
}

void UPauseMenu::NativeDestruct()
{
	// PauseMenu Level Sequence 정지
	const AAuraGameStateBase* AuraGameStateBase = GetAuraGameStateBaseChecked();
	if (AuraGameStateBase->PauseMenuLevelSequenceActor && AuraGameStateBase->PauseMenuLevelSequenceActor->SequencePlayer)
	{
		AuraGameStateBase->PauseMenuLevelSequenceActor->SequencePlayer->Stop();
	}
	
	Super::NativeDestruct();
}

void UPauseMenu::OnReturnToGameButtonClicked()
{
	RemoveFromParent();
}

void UPauseMenu::OnOptionsButtonClicked()
{
	// TODO : Implement
}

void UPauseMenu::OnLeaveGameButtonClicked()
{
	if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>() : nullptr)
	{
		// DestroySession이 끝날 때 호출되는 콜백 함수 연동
		MultiplayerSessionsSubsystem->AuraOnDestroySessionCompleteDelegate.AddUObject(this, &ThisClass::OnDestroySessionComplete);

		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void UPauseMenu::OnDestroySessionComplete(bool bWasSuccessful) const
{
	if (bWasSuccessful)
	{
		if (GetGameInstance())
		{
			GetGameInstance()->ReturnToMainMenu();
		}
	}
	else
	{
		// 실패하면 재시도
		if (UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>() : nullptr)
		{
			MultiplayerSessionsSubsystem->DestroySession();
		}
	}
}

void UPauseMenu::OnExitAuraButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetOwningPlayer(), GetOwningPlayer(), EQuitPreference::Quit, false);
}
