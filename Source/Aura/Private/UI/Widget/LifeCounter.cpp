// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/LifeCounter.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Game/AuraGameStateBase.h"
#include "Player/AuraPlayerController.h"

ULifeCounter::ULifeCounter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RightPadding = 8.f;
}

void ULifeCounter::NativeConstruct()
{
	Super::NativeConstruct();

	if (const AAuraGameStateBase* AuraGameStateBase = GetWorld() ? GetWorld()->GetGameState<AAuraGameStateBase>() : nullptr)
	{
		for (int32 i = 1; i <= AuraGameStateBase->TotalLifeCount; ++i)
		{
			if (UImage* Image = NewObject<UImage>(this, UImage::StaticClass()))
			{
				Image->SetBrush(HeartBrush);
				if (UHorizontalBoxSlot* BoxSlot = HorizontalBox->AddChildToHorizontalBox(Image))
				{
					BoxSlot->SetPadding(FMargin(0.f, 0.f, RightPadding, 0.f));
				}
			}
		}
	}

	AAuraPlayerController* AuraPC = CastChecked<AAuraPlayerController>(GetOwningPlayer());
	AuraPC->OnRespawnStartedDelegate.AddUObject(this, &ThisClass::OnRespawnStarted);
}

void ULifeCounter::OnRespawnStarted(float RespawnTimerEndSeconds)
{
	// Heart Image Widget을 Blank Heart로 변경
	if (UImage* Image = Cast<UImage>(HorizontalBox->GetChildAt(UsedLifeCount)))
	{
		Image->SetBrush(BlankHeartBrush);
		++UsedLifeCount;
	}
}
