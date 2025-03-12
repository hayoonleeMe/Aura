// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUserSettings/AuraGameUserSettings.h"

#include "Kismet/GameplayStatics.h"

void UAuraGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	bFirstRun = true;
	BrightnessValue = 50.f;
	MasterVolumeValue = 100.f;
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

	// TODO : Custom 옵션 적용 
	
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
