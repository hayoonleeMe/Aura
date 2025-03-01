// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/GlobeProgressBar.h"

#include "Components/ProgressBar.h"

void UGlobeProgressBar::NativeConstruct()
{
	Super::NativeConstruct();

	InterpSpeed = 5.f;
}

void UGlobeProgressBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const float Percent = FMath::FInterpTo(ProgressBar_Globe->GetPercent(), TargetPercent, InDeltaTime, InterpSpeed);
	ProgressBar_Globe->SetPercent(Percent);
}
