// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToolTip_AttributeRow.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class AURA_API UToolTip_AttributeRow : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ToolTip;

	FText Description;
};
