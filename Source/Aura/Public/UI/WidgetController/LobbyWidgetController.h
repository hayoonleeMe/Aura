// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetController.h"
#include "LobbyWidgetController.generated.h"

class FOnlineFriend;
class UMultiplayerSessionsSubsystem;

// 초대할 수 있는 친구 정보를 전달하는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInvitableFriendSignature, int32, Index, const FString&, FriendName);

/**
 * 
 */
UCLASS()
class AURA_API ULobbyWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FOnInvitableFriendSignature OnInvitableFriendDelegate;


	// OnlineSubsystem Steam Interface를 이용해 초대할 수 있는 스팀 친구 목록을 불러온다.
	UFUNCTION(BlueprintCallable)
	void RequestInvitableFriendsList();

	// Lobby Widget에서 선택한 친구 유저에게 게임 초대를 전송한다.
	UFUNCTION(BlueprintCallable)
	void SendInvite(int32 FriendIndex);


	// 로비를 나가 MainMenu로 이동
	UFUNCTION(BlueprintCallable)
	void ReturnToMainMenu();

	// Server Travel to Game Level, server only
	UFUNCTION(BlueprintCallable)
	void StartGame(const APlayerController* OwningPlayerController);

private:
	UPROPERTY()
	TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;

	// 초대할 수 있는 스팀 친구 목록을 전달받을 때 
	void InvitableFriendsListArrived(bool bWasSuccessful, TArray<TSharedRef<FOnlineFriend>> FriendsList);

	// 초대할 수 있는 친구 목록 캐시
	TArray<TSharedRef<FOnlineFriend>> CachedFriendsList;

	// Destroy Session 작업이 완료됐을 때의 콜백 함수
	void OnDestroySessionComplete(bool bWasSuccessful) const;
};
