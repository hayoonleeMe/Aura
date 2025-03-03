// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraUserWidget.h"
#include "PauseMenu.generated.h"

class USquareButton;

/**
 * 
 */
UCLASS()
class AURA_API UPauseMenu : public UAuraUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_ReturnToGame;

	UFUNCTION()
	void OnReturnToGameButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_Options;

	UFUNCTION()
	void OnOptionsButtonClicked();
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_LeaveGame;

	UFUNCTION()
	void OnLeaveGameButtonClicked();

	// DestroySession() 작업이 완료됐을 때 호출되는 콜백 함수
	void OnDestroySessionComplete(bool bWasSuccessful) const;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_ExitAura;

	UFUNCTION()
	void OnExitAuraButtonClicked();
};
