// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraUserWidget.h"
#include "LifeCounter.generated.h"

class UHorizontalBox;

/**
 * 
 */
UCLASS()
class AURA_API ULifeCounter : public UAuraUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox;

	UPROPERTY(EditDefaultsOnly)
	FSlateBrush HeartBrush;
	
	UPROPERTY(EditDefaultsOnly)
	FSlateBrush BlankHeartBrush;

	UPROPERTY(EditDefaultsOnly)
	float RightPadding;

	void OnRespawnStarted(float RespawnTimerEndSeconds);

	int32 UsedLifeCount = 0;
};
