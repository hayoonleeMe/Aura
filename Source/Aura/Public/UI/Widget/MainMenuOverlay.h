// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuOverlay.generated.h"

class USquareButton;
/**
 * 
 */
UCLASS()
class AURA_API UMainMenuOverlay : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_Start;

	UFUNCTION()
	void OnStartButtonClicked();
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_Exit;

	UFUNCTION()
	void OnExitButtonClicked();

	void OnCreateSessionComplete(bool bWasSuccessful) const;
};
