// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/ToolTip_XPBar.h"

#include "Components/RichTextBlock.h"
#include "Kismet/KismetMathLibrary.h"

void UToolTip_XPBar::NativeConstruct()
{
	Super::NativeConstruct();

	{
		const FString Format = TEXT("<Tan>Level {Level} </><Normal>({Percent}%)</>");
		const int32 Percent = FMath::TruncToInt32(UKismetMathLibrary::SafeDivide(XP, XPRequirement) * 100.f);
		const FText Text = FText::Format(FTextFormat::FromString(Format), Level, Percent);
		Text_Level->SetText(Text);
	}
	{
		const FString Format = TEXT("<Tan>XP: </><Normal>{XP} </><Tan>/ Next Level: </><Normal>{XPRequirement}</>");
		const FText Text = FText::Format(FTextFormat::FromString(Format), FMath::TruncToInt32(XP), FMath::TruncToInt32(XPRequirement));
		Text_XP->SetText(Text);
	}
}
