// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LifeCounter.generated.h"

class UHorizontalBox;

/**
 * 
 */
UCLASS()
class AURA_API ULifeCounter : public UUserWidget
{
	GENERATED_BODY()

public:
	ULifeCounter(const FObjectInitializer& ObjectInitializer);

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
