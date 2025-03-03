// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RespawnTimer.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class AURA_API URespawnTimer : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

	float RespawnTimerEndSeconds = 0.f;

private:
	float RemainSeconds = 0.f;
	float RespawnTimeSeconds = 0.f;
};
