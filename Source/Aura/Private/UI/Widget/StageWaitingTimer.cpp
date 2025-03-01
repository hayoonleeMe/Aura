// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/StageWaitingTimer.h"

#include "AuraBlueprintLibrary.h"
#include "Components/TextBlock.h"

void UStageWaitingTimer::NativeConstruct()
{
	Super::NativeConstruct();
	
	TranslationTimeSeconds = 1.5f;
	
	Text_StageNumber->SetText(FText::AsNumber(StageNumber));

	PlayAnimationForward(PopUpAnimation);

	if (GEngine && GEngine->GameViewport)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		ViewportHalfY = ViewportSize.Y * 0.5f;
		SetRenderTranslation({ 0.f, ViewportHalfY });
	}
	
	if (GetWorld())
	{
		UpdateRemainTimeSeconds();
		GetWorld()->GetTimerManager().SetTimer(WaitingTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::UpdateRemainTimeSeconds), 1.f, true, 0.f);
	}
}

void UStageWaitingTimer::NativeDestruct()
{
	Super::NativeDestruct();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaitingTimerHandle);
	}
}

void UStageWaitingTimer::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Translate widget upward for TranslationTimeSeconds
	if (Alpha < TranslationTimeSeconds)
	{
		Alpha += InDeltaTime;
		const float NewY = FMath::Lerp(ViewportHalfY, 0.f, Alpha / TranslationTimeSeconds);
		SetRenderTranslation({ 0.f, NewY });
	}
}

void UStageWaitingTimer::UpdateRemainTimeSeconds() const
{
	const float ServerTimeSeconds = UAuraBlueprintLibrary::GetServerWorldTimeSecondsAsFloat(GetWorld());
	if (ServerTimeSeconds > 0.0)
	{
		// 남은 시간 업데이트
		const float RemainSeconds = WaitingTimerEndSeconds - ServerTimeSeconds;
		
		FNumberFormattingOptions Options;
		Options.MinimumFractionalDigits = 0;
		Options.MaximumFractionalDigits = 0;
		Text_RemainSeconds->SetText(FText::AsNumber(RemainSeconds, &Options));

		if (RemainSeconds < 5.5)
		{
			// 마지막 5초는 Red Text
			Text_RemainSeconds->SetColorAndOpacity(FLinearColor::Red);
		}
	}
}
