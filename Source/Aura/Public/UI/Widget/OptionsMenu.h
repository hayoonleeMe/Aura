// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsMenu.generated.h"

class USoundOptionMenu;
class UGraphicsOptionMenu;
class UOptionTab;
class UWidgetSwitcher;
class USquareButton;

/**
 * 
 */
UCLASS()
class AURA_API UOptionsMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionTab> OptionTab_Graphics;
	
	UFUNCTION()
	void OnGraphicsTabClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionTab> OptionTab_Sound;

	UFUNCTION()
	void OnSoundTabClicked();

	UPROPERTY()
	TArray<UOptionTab*> OptionTabs;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UGraphicsOptionMenu> GraphicsOptionMenu;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USoundOptionMenu> SoundOptionMenu; 

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_Save;

	UFUNCTION()
	void OnSaveButtonClicked();
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_Reset;

	UFUNCTION()
	void OnResetButtonClicked();
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_Close;

	UFUNCTION()
	void OnCloseButtonClicked();
};
