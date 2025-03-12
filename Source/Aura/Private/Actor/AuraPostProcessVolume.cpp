// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraPostProcessVolume.h"

#include "GameUserSettings/AuraGameUserSettings.h"
#include "Kismet/KismetMathLibrary.h"

AAuraPostProcessVolume::AAuraPostProcessVolume()
{
	// Screen Brightness를 적용하기 위해 설정 
	Settings.bOverride_AutoExposureBias = true;
}

void AAuraPostProcessVolume::ApplyBrightness(float BrightnessValue)
{
	Settings.AutoExposureBias = BrightnessValue;
}

void AAuraPostProcessVolume::BeginPlay()
{
	Super::BeginPlay();

	// UAuraGameUserSettings의 Screen Brightness 옵션을 사용하도록 초기화
	const UAuraGameUserSettings* AuraGameUserSettings = CastChecked<UAuraGameUserSettings>(GEngine ? GEngine->GetGameUserSettings() : nullptr);
	ApplyBrightness(AuraGameUserSettings->GetClampedBrightnessValue());
}
