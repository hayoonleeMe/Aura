// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobeProgressBar.h"
#include "ManaGlobe.generated.h"

class UToolTip_CurrentMaxValue;

/**
 * 
 */
UCLASS()
class AURA_API UManaGlobe : public UGlobeProgressBar
{
	GENERATED_BODY()

public:
	UManaGlobe(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	virtual void BroadcastInitialValues() override;

public:
	void UpdateMana(float NewValue);
	void UpdateMaxMana(float NewValue);

	float Mana = 0.f;
	float MaxMana = 0.f;

private:
	UFUNCTION()
	UWidget* GetTooltipWidget();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ToolTipWidgetClass;

	UPROPERTY()
	TObjectPtr<UToolTip_CurrentMaxValue> CachedToolTipWidget;

	void UpdateToolTipWidget() const;
};
