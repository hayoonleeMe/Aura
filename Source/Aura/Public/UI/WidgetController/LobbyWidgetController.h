// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetController.h"
#include "LobbyWidgetController.generated.h"

class UMultiplayerSessionsSubsystem;

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


	// 로비를 나가 MainMenu로 이동
	UFUNCTION(BlueprintCallable)
	void ReturnToMainMenu();

	// Server Travel to Game Level, server only
	UFUNCTION(BlueprintCallable)
	void StartGame(const APlayerController* OwningPlayerController);

private:
	UPROPERTY()
	TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;

	// Destroy Session 작업이 완료됐을 때의 콜백 함수
	void OnDestroySessionComplete(bool bWasSuccessful) const;
};
