// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XPBar.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class AURA_API UXPBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UXPBar(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_XP;

	UFUNCTION()
	UWidget* GetProgressBarXPTooltipWidget();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ToolTipWidgetClass;

	void UpdateXPChange(float InXP);
	void UpdateLevelChange(int32 InLevel);
	
	float XP = 0.f;
	float XPRequirement = 0.f;
	int32 Level = 0;
};
