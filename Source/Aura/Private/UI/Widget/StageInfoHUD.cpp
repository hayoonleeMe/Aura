// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/StageInfoHUD.h"

#include "Components/TextBlock.h"
#include "Player/AuraPlayerController.h"

void UStageInfoHUD::NativeConstruct()
{
	Super::NativeConstruct();

	Text_StageNumber->SetText(FText::AsNumber(StageNumber));
	Text_RemainCount->SetText(FText::AsNumber(EnemyCount));
	Text_TotalCount->SetText(FText::AsNumber(EnemyCount));

	if (AAuraPlayerController* AuraPC = GetOwningPlayer<AAuraPlayerController>())
	{
		AuraPC->OnEnemyDeadDelegate.BindUObject(this, &ThisClass::OnEnemyDead);
	}

	PlayAnimationForward(PopUpAnimation);
}

void UStageInfoHUD::OnEnemyDead()
{
	PlayAnimationForward(RemainCountTextBlinkAnimation);
	
	--EnemyCount;
	Text_RemainCount->SetText(FText::AsNumber(EnemyCount));

	if (EnemyCount <= 0)
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
