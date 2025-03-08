// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/ToolTip_XPBar.h"

#include "Components/RichTextBlock.h"
#include "Kismet/KismetMathLibrary.h"

void UToolTip_XPBar::NativeConstruct()
{
	Super::NativeConstruct();

	const int32 Percent = FMath::TruncToInt32(UKismetMathLibrary::SafeDivide(XP, XPRequirement) * 100.f);
	const FString LevelString = FString::Printf(TEXT("<Tan>Level %d </><Normal>(%d%%)</>"), Level, Percent);
	Text_Level->SetText(FText::FromString(LevelString));

	const FString XPString = FString::Printf(TEXT("<Tan>XP: </><Normal>%d </><Tan>/ Next Level: </><Normal>%d</>"), FMath::TruncToInt32(XP), FMath::TruncToInt32(XPRequirement));
	Text_XP->SetText(FText::FromString(XPString));
}
