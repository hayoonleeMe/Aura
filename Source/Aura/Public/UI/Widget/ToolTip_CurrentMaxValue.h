// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToolTip_CurrentMaxValue.generated.h"

class UTextBlock;

/**
 * Current Value와 Max Value를 표시하는 ToolTip Widget  
 */
UCLASS()
class AURA_API UToolTip_CurrentMaxValue : public UUserWidget
{
	GENERATED_BODY()

public:
	UToolTip_CurrentMaxValue(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_CurrentValue;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_MaxValue;

	void UpdateValues(float CurrentValue, float MaxValue) const;

private:
	FNumberFormattingOptions NumberFormattingOptions;
};
