// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GlobeProgressBar.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class AURA_API UGlobeProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UGlobeProgressBar(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Globe;

	UPROPERTY(EditDefaultsOnly)
	float InterpSpeed;
	
	float TargetPercent = 0.f;
};
