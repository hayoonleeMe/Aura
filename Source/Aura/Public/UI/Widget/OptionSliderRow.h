// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionSliderRow.generated.h"

class USlider;
class UTextBlock;

/**
 * 
 */
UCLASS()
class AURA_API UOptionSliderRow : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Value;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USlider> Slider;

	// 초기 Slider 설정
	void InitializeSlider(float MinValue, float MaxValue, float StepSize, int32 Value) const;

	// Slider 값 설정
	void SetSliderValue(int32 Value) const;

	UFUNCTION()
	void OnSliderValueChanged(float Value);
};
