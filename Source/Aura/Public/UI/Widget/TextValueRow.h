// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraUserWidget.h"
#include "GameplayTagContainer.h"
#include "TextValueRow.generated.h"

class UToolTip_AttributeRow;
struct FAttributeInfo;
class UFramedValue;
class UTextBlock;

/**
 * 
 */
UCLASS()
class AURA_API UTextValueRow : public UAuraUserWidget
{
	GENERATED_BODY()

public:
	UTextValueRow(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Label;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ToolTipWidgetClass;
	
	UFUNCTION()
	UWidget* GetTextLabelTooltipWidget();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UFramedValue> FramedValue;
	
	FText Description;
	FGameplayTag AttributeTag;

	void UpdateAttributeValueChange(const FAttributeInfo& AttributeInfo);
};
