// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/OptionComboBoxRow.h"

#include "Components/ComboBoxKey.h"
#include "Components/TextBlock.h"

void UOptionComboBoxRow::NativeConstruct()
{
	Super::NativeConstruct();

	ComboBox->OnGenerateContentWidget.BindDynamic(this, &ThisClass::GetContentWidget);
	ComboBox->OnGenerateItemWidget.BindDynamic(this, &ThisClass::GetItemWidget);
	ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged);
}

bool UOptionComboBoxRow::IsComboBoxOpen() const
{
	return ComboBox && ComboBox->IsOpen();
}

UWidget* UOptionComboBoxRow::GetContentWidget(FName Item)
{
	if (Item.IsNone())
	{
		return nullptr;
	}
	
	ContentWidgetTextBlock->SetText(FText::FromName(Item));
	return ContentWidgetTextBlock;
}

UWidget* UOptionComboBoxRow::GetItemWidget(FName Item)
{
	if (Item.IsNone() || !ItemWidgets.Contains(Item))
	{
		return nullptr;
	}
	
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
	// SelectedOption이 None으로 설정되어 OnSelectionChanged 이벤트가 발생함
	ComboBox->ClearOptions();
	
	for (const FName& Option : Options)
	{
		ComboBox->AddOption(Option);
		
		// Caching Item Widget
		if (!ItemWidgets.Contains(Option))
		{
			ItemWidgets.Add(Option, MakeItemTextBlock(Option));
		}
	}

	// Caching Content Widget
	if (!ContentWidgetTextBlock)
	{
		ContentWidgetTextBlock = MakeItemTextBlock(FName());
		if (ContentWidgetTextBlock)
		{
			ContentWidgetTextBlock->SetClipping(EWidgetClipping::ClipToBoundsAlways);
		}
	}

	// SelectedOption이 Options에 존재하지 않으면 마지막 옵션을 선택
	FName ValidSelectedOption = SelectedOption;
	if (!Options.Contains(SelectedOption))
	{
		ValidSelectedOption = Options.Last();
	}

	ComboBox->SetSelectedOption(ValidSelectedOption);
}

void UOptionComboBoxRow::SetSelectedOption(const FName& SelectedOption) const
{
	ComboBox->SetSelectedOption(SelectedOption); 
}

void UOptionComboBoxRow::OnSelectionChanged(FName SelectedItem, ESelectInfo::Type SelectionType)
{
	// ComboBox->ClearOptions()가 호출되면서 SelectedOption이 None으로 설정되어 이 콜백 함수가 호출되는 경우 무시
	if (SelectedItem.IsNone())
	{
		return;
	}

	if (PrevSelectedOption != SelectedItem)
	{
		if (ItemWidgets.Contains(PrevSelectedOption) && ItemWidgets[PrevSelectedOption])
		{
			// 이전에 선택된 옵션 Item의 Text Color를 기본으로 업데이트
			ItemWidgets[PrevSelectedOption]->SetColorAndOpacity(ComboBox->GetItemStyle().TextColor);
		}
		if (ItemWidgets.Contains(SelectedItem) && ItemWidgets[SelectedItem])
		{
			// 새로 선택된 옵션 Item의 Text Color를 선택됨을 나타내는 전용 색상으로 업데이트
			ItemWidgets[SelectedItem]->SetColorAndOpacity(ComboBox->GetItemStyle().SelectedTextColor);
		}
		PrevSelectedOption = SelectedItem;
	}
}
