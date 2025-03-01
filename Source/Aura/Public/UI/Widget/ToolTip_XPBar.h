// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToolTip_XPBar.generated.h"

class URichTextBlock;

/**
 * 
 */
UCLASS()
class AURA_API UToolTip_XPBar : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URichTextBlock> Text_Level;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URichTextBlock> Text_XP;

	int32 Level = 0;
	float XP = 0.f;
	float XPRequirement = 0.f;
};
