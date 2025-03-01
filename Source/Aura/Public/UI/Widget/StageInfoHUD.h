// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageInfoHUD.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class AURA_API UStageInfoHUD : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_StageNumber;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_RemainCount;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_TotalCount;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopUpAnimation;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> RemainCountTextBlinkAnimation;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ClearStageAnimation;

	int32 StageNumber = 0;
	int32 EnemyCount = 0;

	void OnEnemyDead();
};
