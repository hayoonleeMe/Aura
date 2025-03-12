// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

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

	// 전체 음량 변경
	void SetMasterSoundVolume(float Volume) const;
	
	// LoadingOverlay Widget을 생성하거나 제거
	// 위젯이 레벨이 변경되도 제거되지 않도록 구현
	void DisplayLoadingOverlay(bool bDisplay);
	
private:
	// 유저가 세션 초대를 수락했을 때 호출할 콜백 함수
	void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);

	// 위 콜백 함수를 등록한 델레게이트의 핸들
	FDelegateHandle OnSessionUserInviteAcceptedDelegateHandle;
	
	// AuraOnDestroySessionCompleteDelegate 핸들
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	// AuraOnJoinSessionCompleteDelegate 핸들
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	// 로딩 중임을 나타내는 LoadingOverlay Widget
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> LoadingOverlayClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> LoadingOverlay;

	// ============================================================================
	// Sound
	// ============================================================================

    UPROPERTY(EditAnywhere, Category="Aura|Sound")
    TObjectPtr<USoundClass> MasterSoundClass;
};
