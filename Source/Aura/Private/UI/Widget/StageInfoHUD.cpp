// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/StageInfoHUD.h"

#include "Components/TextBlock.h"
#include "Player/AuraPlayerController.h"

void UStageInfoHUD::InitializeStageInfoHUD(int32 InStageNumber, int32 InTotalEnemyCount)
{
	if (AAuraPlayerController* AuraPC = GetOwningPlayer<AAuraPlayerController>())
	{
		AuraPC->OnEnemyDeadDelegate.BindUObject(this, &ThisClass::OnEnemyDead);
	}

	StageNumber = InStageNumber;
	TotalEnemyCount = RemainEnemyCount = InTotalEnemyCount;
	
	Text_StageNumber->SetText(FText::AsNumber(StageNumber));
	Text_RemainCount->SetText(FText::AsNumber(RemainEnemyCount));
	Text_TotalCount->SetText(FText::AsNumber(TotalEnemyCount));

	PlayAnimationForward(PopUpAnimation);
}

void UStageInfoHUD::UpdateTotalEnemyCount(int32 InTotalEnemyCount)
{
	RemainEnemyCount += InTotalEnemyCount - TotalEnemyCount;
	TotalEnemyCount = InTotalEnemyCount;
	Text_RemainCount->SetText(FText::AsNumber(RemainEnemyCount));
	Text_TotalCount->SetText(FText::AsNumber(TotalEnemyCount));
	
	PlayAnimationForward(RemainCountTextBlinkAnimation);
	PlayAnimationForward(TotalCountTextBlinkAnimation);
}

void UStageInfoHUD::OnEnemyDead()
{
	PlayAnimationForward(RemainCountTextBlinkAnimation);
	
	--RemainEnemyCount;
	Text_RemainCount->SetText(FText::AsNumber(RemainEnemyCount));

	if (RemainEnemyCount <= 0)
	{
		if (GetWorld())
		{
			// 1초 뒤 ClearStageAnimation 실행
			FTimerHandle AnimTimer;
			GetWorld()->GetTimerManager().SetTimer(AnimTimer, FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				PlayAnimationForward(ClearStageAnimation);
			}), 1.f, false);
		}
	}
}




