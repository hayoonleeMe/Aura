// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionButtonRow.generated.h"

class USquareButton;

/**
 * 
 */
UCLASS()
class AURA_API UOptionButtonRow : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<USquareButton> Button_Option;

	UFUNCTION()
	void OnButtonClicked();
};
