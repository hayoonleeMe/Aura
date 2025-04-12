// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToolTip_Beacon_StartStage.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class AURA_API UToolTip_Beacon_StartStage : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetKeyText(const FString& KeyString) const;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Key;
};
