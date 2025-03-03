// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/GameEndAlert.h"

void UGameEndAlert::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimationForward(PopUpAnimation);
}
