// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHUD.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AMainMenuHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UUserWidget> MainMenuOverlay;
	
	UPROPERTY(EditDefaultsOnly, Category="MainMenu")
	TSubclassOf<UUserWidget> MainMenuOverlayClass;
};
