// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/TextValueRow.h"

#include "Components/TextBlock.h"
#include "Data/AttributeConfig.h"
#include "UI/Widget/FramedValue.h"
#include "UI/Widget/ToolTip_AttributeRow.h"

void UTextValueRow::NativeConstruct()
{
	Super::NativeConstruct();
	
	check(ToolTipWidgetClass);

	ToolTipWidgetDelegate.BindDynamic(this, &ThisClass::GetTextLabelTooltipWidget);
	SynchronizeProperties();	// ToolTipWidgetDelegate 적용을 위해 동기화
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
