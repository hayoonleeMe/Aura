// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

class UMultiplayerSessionsSubsystem;

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;
	
private:
	// 유저가 세션 초대를 수락했을 때 호출할 콜백 함수
	void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult) const;

	// 위 콜백 함수를 등록한 델레게이트의 핸들
	FDelegateHandle OnSessionUserInviteAcceptedDelegateHandle;
};
