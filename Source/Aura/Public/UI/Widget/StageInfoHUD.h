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
	TObjectPtr<UWidgetAnimation> TotalCountTextBlinkAnimation;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ClearStageAnimation;

	void InitializeStageInfoHUD(int32 InStageNumber, int32 InTotalEnemyCount);
	void UpdateTotalEnemyCount(int32 InTotalEnemyCount);
	
private:
	int32 StageNumber = 0;
	int32 RemainEnemyCount = 0;
	int32 TotalEnemyCount = 0;

	void OnEnemyDead();
};
