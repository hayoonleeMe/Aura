// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageStartAlert.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class AURA_API UStageStartAlert : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_StageNumber;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopAndFadeOutAnimation;

	UFUNCTION()
	void OnPopAndFadeOutAnimationFinished();

	int32 StageNumber = 0;
};
