// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHUD.generated.h"

// Start Button을 활성화하는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnableStartButtonSignature);

/**
 * 
 */
UCLASS()
class AURA_API AMainMenuHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FEnableStartButtonSignature EnableStartButtonDelegate;

	// 세션을 생성하고 로비로 이동
	UFUNCTION(BlueprintCallable)
	void CreateSession() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UUserWidget> MainMenuOverlay;
	
	UPROPERTY(EditDefaultsOnly, Category="MainMenu")
	TSubclassOf<UUserWidget> MainMenuOverlayClass;
};
