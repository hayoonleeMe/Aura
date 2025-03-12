// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/OptionComboBoxRow.h"

#include "Components/Border.h"
#include "Components/ComboBoxKey.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

void UOptionComboBoxRow::NativeConstruct()
{
	Super::NativeConstruct();

	ComboBox->OnGenerateContentWidget.BindDynamic(this, &ThisClass::GetContentWidget);
	ComboBox->OnGenerateItemWidget.BindDynamic(this, &ThisClass::GetItemWidget);
	ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged);
}

UWidget* UOptionComboBoxRow::GetContentWidget(FName Item)
{
	ContentWidgetTextBlock->SetText(FText::FromName(Item));
	return ContentWidgetTextBlock;
}

UWidget* UOptionComboBoxRow::GetItemWidget(FName Item)
{
	return ItemWidgets[Item];
}

UTextBlock* UOptionComboBoxRow::MakeItemTextBlock(const FName& Option)
{
	if (UTextBlock* TextBlock = NewObject<UTextBlock>(this, UTextBlock::StaticClass()))
	{
		TextBlock->SetFont(ItemFont);
		TextBlock->SetText(FText::FromName(Option));
		TextBlock->SetJustification(ETextJustify::Left);
		TextBlock->SetColorAndOpacity(ComboBox->GetItemStyle().TextColor);
		return TextBlock;
	}
	return nullptr;
}

void UOptionComboBoxRow::SetComboBoxOptions(const TArray<FName>& Options, const FName& SelectedOption)
{
	ItemWidgets.Empty();
	
	for (const FName& Option : Options)
	{
		ComboBox->AddOption(Option);
		// Caching Item Widget
		ItemWidgets.Add(Option, MakeItemTextBlock(Option));
	}

	// Caching Content Widget
	ContentWidgetTextBlock = MakeItemTextBlock(SelectedOption);
	if (ContentWidgetTextBlock)
	{
		ContentWidgetTextBlock->SetClipping(EWidgetClipping::ClipToBoundsAlways);
	}
	
	ComboBox->SetSelectedOption(SelectedOption);
}

void UOptionComboBoxRow::OnSelectionChanged(FName SelectedItem, ESelectInfo::Type SelectionType)
{
	// SelectedItem 강조를 위한 Text 색상 변경
	for (const auto& Pair : ItemWidgets)
	{
		if (Pair.Key == SelectedItem)
		{
			Pair.Value->SetColorAndOpacity(ComboBox->GetItemStyle().SelectedTextColor);
		}
		else
		{
			Pair.Value->SetColorAndOpacity(ComboBox->GetItemStyle().TextColor);
		}
	}
}
