// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LobbyHUD.generated.h"

class ULobbyWidgetController;
class UAuraUserWidget;

/**
 * Lobby Overlay를 표시할 HUD
 */
UCLASS()
class AURA_API ALobbyHUD : public AHUD
{
	GENERATED_BODY()

public:
	ULobbyWidgetController* GetLobbyWidgetController() const { return LobbyWidgetController; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> LobbyOverlay;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Lobby")
	TSubclassOf<UAuraUserWidget> LobbyOverlayClass;

	UPROPERTY()
	TObjectPtr<ULobbyWidgetController> LobbyWidgetController;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Lobby")
	TSubclassOf<ULobbyWidgetController> LobbyWidgetControllerClass;
};
