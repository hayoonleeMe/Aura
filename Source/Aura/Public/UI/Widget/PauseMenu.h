// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ClosableWidget.h"
#include "PauseMenu.generated.h"

class USquareButton;

/**
 * 
 */
UCLASS()
class AURA_API UPauseMenu : public UClosableWidget
{
	GENERATED_BODY()

public:
	/* Begin IClosableWidgetInterface */
	virtual void CloseMenu() override;
	/* End IClosableWidgetInterface */

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_ReturnToGame;

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
