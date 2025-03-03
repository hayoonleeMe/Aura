// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/RespawnTimer.h"

#include "AuraBlueprintLibrary.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"

void URespawnTimer::NativeConstruct()
{
	Super::NativeConstruct();

	const float ServerTimeSeconds = UAuraBlueprintLibrary::GetServerWorldTimeSecondsAsFloat(GetWorld());
	if (ServerTimeSeconds > 0.0)
	{
		RespawnTimeSeconds = RespawnTimerEndSeconds - ServerTimeSeconds;
		RemainSeconds = RespawnTimeSeconds;
	}
}

void URespawnTimer::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (RespawnTimeSeconds > 0.f)
	{
		ProgressBar->SetPercent(RemainSeconds / RespawnTimeSeconds);
		RemainSeconds -= InDeltaTime;
	}
	if (RemainSeconds <= 0.f)
	{
		RemoveFromParent();
	}
}
