// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUserSettings/AuraGameUserSettings.h"

#include "Actor/AuraPostProcessVolume.h"
#include "Game/AuraGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UAuraGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	bFirstRun = true;
	BrightnessValue = static_cast<int32>(MaxUIBrightness * 0.5f);
	MasterVolumeValue = static_cast<int32>(MaxUIVolume);
}

float UAuraGameUserSettings::GetClampedBrightnessValue() const
{
	return UKismetMathLibrary::MapRangeClamped(BrightnessValue, 0.f, MaxUIBrightness, MinAppliedBrightness, MaxAppliedBrightness);
}

float UAuraGameUserSettings::GetClampedVolumeValue() const
{
	return UKismetMathLibrary::MapRangeClamped(MasterVolumeValue, 0.f, MaxUIVolume, 0.f, MaxAppliedVolume);
}

void UAuraGameUserSettings::InitializeGameUserSettings(const UWorld* World)
{
	if (bFirstRun)
	{
		// 게임 첫 실행이면 초기 설정 저장하고 Graphic Quality 옵션 자동 설정
		bFirstRun = false;
		ApplyCustomSettings(false, GetWorld());
		SetGraphicOptionByAutoDetect();
	}
	else
	{
		// 저장된 옵션 로드하고 적용
		LoadSettings();
		ApplyCustomSettings(false, World);
	}
}

void UAuraGameUserSettings::ApplyCustomSettings(bool bCheckForCommandLineOverrides, const UWorld* World)
{
	Super::ApplySettings(bCheckForCommandLineOverrides);

	if (World)
	{
		// Sound
		if (const UAuraGameInstance* AuraGameInstance = World->GetGameInstance<UAuraGameInstance>())
		{
			AuraGameInstance->SetMasterSoundVolume(GetClampedVolumeValue());
		}

		// Screen Brightness
		if (AAuraPostProcessVolume* AuraPostProcessVolume = Cast<AAuraPostProcessVolume>(UGameplayStatics::GetActorOfClass(World, AAuraPostProcessVolume::StaticClass())))
		{
			// 인게임에서 변경한 경우 월드에서 찾을 수 있으므로 옵션 설정
			AuraPostProcessVolume->ApplyBrightness(GetClampedBrightnessValue());
		}
	}
}

void UAuraGameUserSettings::SetGraphicOptionByAutoDetect()
{
	// Store benchmark result in ScalabilityQuality
	RunHardwareBenchmark();

	// ResolutionQuality는 항상 100으로 고정
	SetResolutionScaleValueEx(100.f);

	// Apply settings in ScalabilityQuality and Save settings
	ApplyHardwareBenchmarkResults();
}

void UAuraGameUserSettings::SetOverallScalabilityLevelIgnoringResolutionQuality(int32 Value)
{
	SetOverallScalabilityLevel(Value);

	// ResolutionQuality는 항상 100으로 고정
	SetResolutionScaleValueEx(100.f);
}

int32 UAuraGameUserSettings::GetOverallScalabilityLevelIgnoringResolutionQuality() const
{
	const int32 Target = ScalabilityQuality.ViewDistanceQuality;
	if ((Target == ScalabilityQuality.AntiAliasingQuality) && (Target == ScalabilityQuality.ShadowQuality) && (Target == ScalabilityQuality.GlobalIlluminationQuality) && (Target == ScalabilityQuality.ReflectionQuality) && (Target == ScalabilityQuality.PostProcessQuality) && (Target == ScalabilityQuality.TextureQuality) && (Target == ScalabilityQuality.EffectsQuality) && (Target == ScalabilityQuality.FoliageQuality) && (Target == ScalabilityQuality.ShadingQuality))
	{
		return Target;
	}
	return -1;
}
