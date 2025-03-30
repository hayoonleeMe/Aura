// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobeProgressBar.h"
#include "HealthGlobe.generated.h"

class UToolTip_CurrentMaxValue;
/**
 * 
 */
UCLASS()
class AURA_API UHealthGlobe : public UGlobeProgressBar
{
	GENERATED_BODY()

public:
	UHealthGlobe(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	void UpdateHealth(float NewValue);
	void UpdateMaxHealth(float NewValue);
	
	float Health = 0.f;
	float MaxHealth = 0.f;

private:
	UFUNCTION()
	UWidget* GetTooltipWidget();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ToolTipWidgetClass;

	UPROPERTY()
	TObjectPtr<UToolTip_CurrentMaxValue> CachedToolTipWidget;

	void UpdateToolTipWidget() const;
};
