// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/TextValueRow.h"

#include "Components/TextBlock.h"
#include "Data/AttributeConfig.h"
#include "UI/Widget/FramedValue.h"
#include "UI/Widget/ToolTip_AttributeRow.h"

UTextValueRow::UTextValueRow(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ToolTipWidgetDelegate.BindDynamic(this, &ThisClass::GetTextLabelTooltipWidget);
}

void UTextValueRow::NativeConstruct()
{
	Super::NativeConstruct();
	
	check(ToolTipWidgetClass);
}

UWidget* UTextValueRow::GetTextLabelTooltipWidget()
{
	UToolTip_AttributeRow* ToolTipUserWidget = CreateWidget<UToolTip_AttributeRow>(this, ToolTipWidgetClass);
	ToolTipUserWidget->Description = Description;
	return ToolTipUserWidget;
}

void UTextValueRow::UpdateAttributeValueChange(const FAttributeInfo& AttributeInfo)
{
	Description = AttributeInfo.AttributeDescription;
	Text_Label->SetText(AttributeInfo.AttributeName);
	FramedValue->Text_Value->SetText(FText::AsNumber(AttributeInfo.Value));
}
