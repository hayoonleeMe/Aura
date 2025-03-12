// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionTab.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class AURA_API UOptionTab : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> InternalButton;

	UFUNCTION()
	void Select();
	
	void Deselect();

	UPROPERTY(EditDefaultsOnly)
	FSlateBrush TransparentBrush;
	
	UPROPERTY(EditDefaultsOnly)
	FSlateBrush SelectedBrush;

	FButtonStyle ButtonStyle;
};
