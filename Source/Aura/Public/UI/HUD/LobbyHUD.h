// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LobbyHUD.generated.h"

class ULobbyOverlay;

/**
 * Lobby Overlay를 표시할 HUD
 */
UCLASS()
class AURA_API ALobbyHUD : public AHUD
{
	GENERATED_BODY()

public:
	// 로비에 새로운 플레이어가 추가됐을 때
	void OnNewPlayerAdded(const FString& PlayerName) const;

	// 로비에서 플레이어가 떠났을 때
	void OnPlayerLeft() const;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<ULobbyOverlay> LobbyOverlay;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura|Lobby")
	TSubclassOf<ULobbyOverlay> LobbyOverlayClass;
};
