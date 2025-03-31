// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;

/**
 * MainMenu, Lobby와 같은 메뉴 레벨에서 UI Input을 설정하는 Player Controller
 */
UCLASS()
class AURA_API AMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	// ============================================================================
	// Input
	// ============================================================================

	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<UInputMappingContext> UIContext;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<UInputAction> IA_CloseUI;

	// CloseUI Input Action이 Started 될 때 호출되는 콜백 함수
	void OnCloseUIActionStarted();
};
