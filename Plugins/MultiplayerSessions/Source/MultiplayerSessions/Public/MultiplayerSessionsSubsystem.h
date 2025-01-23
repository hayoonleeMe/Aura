// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MultiplayerSessionsSubsystem.generated.h"

// Custom Log Category
DECLARE_LOG_CATEGORY_EXTERN(LogMultiplayerSessions, Log, All);

// Custom Log using dark yellow color
#define LOG_DARK_YELLOW(Format, ...) \
	SET_WARN_COLOR(COLOR_DARK_YELLOW) \
	UE_LOG(LogMultiplayerSessions, Log, TEXT("%s"), *FString::Printf(Format, ##__VA_ARGS__)) \
	CLEAR_WARN_COLOR()

// CreateSession 결과를 전달하는 델레게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FAuraOnCreateSessionCompleteSignature, bool /* bWasSuccessful */);

// FindSessions 결과를 전달하는 델레게이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FAuraOnFindSessionsCompleteSignature, const TArray<FOnlineSessionSearchResult>& /* SessionResults */, bool /* bWasSuccessful */);

// FindSessions의 식별된 첫번째 결과만 전달하는 델레게이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FAuraOnFindFirstSessionCompleteSignature, const FOnlineSessionSearchResult& /* SessionResult */, bool /* bWasSuccessful */);

// JoinSession 결과를 전달하는 델레게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FAuraOnJoinSessionCompleteSignature, EOnJoinSessionCompleteResult::Type /* Result */);

// DestroySession 결과를 전달하는 델레게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FAuraOnDestroySessionCompleteSignature, bool /* bWasSuccessful */);

// 초대할 수 있는 친구 목록을 구하는 작업의 결과를 전달하는 델레게이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FInvitableFriendsListSignature, bool /* bWasSuccessful */, TArray<TSharedRef<FOnlineFriend>> /* FriendsList */);

/**
 * OnlineSubsystem Steam API를 이용해 멀티플레이어 세션을 관리하는 서브시스템
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();

	/*
	 * Get Online Subsystem Interface
	 */
	static IOnlineSessionPtr GetSessionInterface();
	static IOnlineFriendsPtr GetFriendsInterface();

	/*
	* To handle session functionality.
	*/
	void CreateSession();
	void FindSessions();
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession();

	// SessionName에 해당하는 NamedOnlineSession 반환
	static FNamedOnlineSession* GetNamedOnlineSession(FName SessionName = NAME_GameSession);

	// SessionName에 해당하는 Session의 정보를 FString으로 합쳐 반환
	static FString GetDebugSessionInfo(FName SessionName = NAME_GameSession);

	/*
	 * 외부에서 Session 기능 종료를 알리는 델레게이트
	 */
	FAuraOnCreateSessionCompleteSignature AuraOnCreateSessionCompleteDelegate;
	FAuraOnFindSessionsCompleteSignature AuraOnFindSessionsCompleteDelegate;
	FAuraOnFindFirstSessionCompleteSignature AuraOnFindFirstSessionCompleteDelegate;
	FAuraOnJoinSessionCompleteSignature AuraOnJoinSessionCompleteDelegate;
	FAuraOnDestroySessionCompleteSignature AuraOnDestroySessionCompleteDelegate;

	// 초대할 수 있는 친구 목록을 요청, 결과는 InvitableFriendsListDelegate에서 브로드캐스트
	void RequestInvitableFriendsList();

	FInvitableFriendsListSignature InvitableFriendsListDelegate;
	
	// FriendId에 해당하는 유저에 세션 초대를 전송
	static void SendSessionInvite(const FUniqueNetId& FriendId);

	// Friend의 정보를 Log로 출력
	static void PrintFriendDebugLog(const FOnlineFriend& Friend);

private:
	/*
	* Internal callbacks for the delegates we'll add to the Online Session Interface delegate list.
	* This don't need to be called outside this class.
	*/
	void OnCreateSessionComplete_Internal(FName SessionName, bool bWasSuccessful);
	void OnFindSessionComplete_Internal(bool bWasSuccessful);
	void OnJoinSessionComplete_Internal(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete_Internal(FName SessionName, bool bWasSuccessful);

	// 가장 최근에 생성된 Session 설정
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;

	// 가장 최근의 세션 검색 정보
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	/*
	 *	To add to the Online Session Interface delegate list.
	 *	We'll bind our MultiplayerSessionsSubsystem internal callbacks to these.
	 */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	// 세션을 생성할 때 이미 세션이 존재하면 그 세션 제거가 완료될 때 다시 세션 생성하기 위한 트리거
	bool bCreateSessionOnDestroy = false;

	// 친구 목록 조회가 끝나면 호출되는 콜백 함수
	void OnReadFriendsComplete(int32 LocalPlayer, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr) const;
};
