// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionComboBoxRow.generated.h"

class UTextBlock;
class UBorder;
class UComboBoxContent;
class UComboBoxKey;

/**
 * 
 */
UCLASS()
class AURA_API UOptionComboBoxRow : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UComboBoxKey> ComboBox;

	UFUNCTION()
	UWidget* GetContentWidget(FName Item);

	UPROPERTY()
	TObjectPtr<UTextBlock> ContentWidgetTextBlock;
	
	UFUNCTION()
	UWidget* GetItemWidget(FName Item);

	UPROPERTY()
	TMap<FName, TObjectPtr<UTextBlock>> ItemWidgets;

	UPROPERTY(EditDefaultsOnly)
	FSlateFontInfo ItemFont;

	UTextBlock* MakeItemTextBlock(const FName& Option);

	void SetComboBoxOptions(const TArray<FName>& Options, const FName& SelectedOption);

	UFUNCTION()
	void OnSelectionChanged(FName SelectedItem, ESelectInfo::Type SelectionType);
};
