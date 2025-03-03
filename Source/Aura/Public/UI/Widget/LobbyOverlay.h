// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyOverlay.generated.h"

class UFriendsListRow;
class FOnlineFriend;
class UMultiplayerSessionsSubsystem;
class UScrollBox;
class UTextBlock;
class USquareButton;

/**
 * 
 */
UCLASS()
class AURA_API ULobbyOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	ULobbyOverlay(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UPROPERTY()
	TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_Start;

	UFUNCTION()
	void OnStartButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_Exit;

	UFUNCTION()
	void OnExitButtonClicked();

	// Destroy Session 작업이 완료됐을 때의 콜백 함수
	void OnDestroySessionComplete(bool bWasSuccessful) const;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_HostPlayerName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ClientPlayerName;

	UPROPERTY(EditDefaultsOnly)
	FString EmptyPlayerName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_FriendsList;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFriendsListRow> FriendsListRowClass;

	FTimerHandle RefreshFriendsListTimerHandle;
	void RequestInvitableFriendsList() const;

	UPROPERTY(EditDefaultsOnly)
	float FriendsListRefreshIntervalSeconds;

	// 초대할 수 있는 스팀 친구 목록을 전달받을 때 
	void InvitableFriendsListArrived(bool bWasSuccessful, TArray<TSharedRef<FOnlineFriend>> FriendsList);

	int32 NumLobbyPlayers = 0;

	// 로비에 새로운 플레이어가 추가됐을 때
	void OnNewPlayerAdded(const FString& PlayerName);

	// 로비에서 플레이어가 떠났을 때
    void OnPlayerLeft();
	
	bool bCanInvite = true;

	void EnableInviteButton(bool bInCanInvite);
};
