// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/StageStartAlert.h"

#include "Components/TextBlock.h"

void UStageStartAlert::NativeConstruct()
{
	Super::NativeConstruct();
	
	Text_StageNumber->SetText(FText::AsNumber(StageNumber));

	// PopAndFadeOutAnimation 실행 후 끝나면 해당 위젯 제거
	FWidgetAnimationDynamicEvent Event;
	Event.BindDynamic(this, &ThisClass::OnPopAndFadeOutAnimationFinished);
	BindToAnimationFinished(PopAndFadeOutAnimation, Event);
	
	PlayAnimationForward(PopAndFadeOutAnimation);
}

void UStageStartAlert::OnPopAndFadeOutAnimationFinished()
{
	RemoveFromParent();
}
