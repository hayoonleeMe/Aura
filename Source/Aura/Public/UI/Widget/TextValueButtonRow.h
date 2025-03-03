// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TextValueRow.h"
#include "TextValueButtonRow.generated.h"

class USquareButton;

/**
 * 
 */
UCLASS()
class AURA_API UTextValueButtonRow : public UTextValueRow
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_Upgrade;

	UFUNCTION()
	void OnUpgradeButtonClicked();
};
