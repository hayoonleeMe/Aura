// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageWaitingTimer.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class AURA_API UStageWaitingTimer : public UUserWidget
{
	GENERATED_BODY()

public:
	UStageWaitingTimer(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_StageNumber;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_RemainSeconds;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopUpAnimation;
	
	int32 StageNumber = 0;
	float WaitingTimerEndSeconds = 0.f;

	FTimerHandle WaitingTimerHandle;
	void UpdateRemainTimeSeconds() const;

private:
	float Alpha = 0.f;
	float ViewportHalfY = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float TranslationTimeSeconds;

	FNumberFormattingOptions NumberFormattingOptions;
};
