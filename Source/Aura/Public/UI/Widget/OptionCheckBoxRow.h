// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionCheckBoxRow.generated.h"

class UCheckBox;

/**
 * 
 */
UCLASS()
class AURA_API UOptionCheckBoxRow : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCheckBox> CheckBox;

	bool bChecked = false;
};
