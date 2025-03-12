// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "AuraGameUserSettings.generated.h"

/**
 * 유저 별 게임 설정을 config로 저장하는 Custom UGameUserSettings 클래스
 * 설정은 Config/GameUserSettings.ini에 저장된다.
 */
UCLASS()
class AURA_API UAuraGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	// 초기 설정
	virtual void SetToDefaults() override;
	
	void SetBrightnessValue(int32 InValue) { BrightnessValue = InValue; }
	int32 GetBrightnessValue() const { return BrightnessValue; }
	float GetClampedBrightnessValue() const;

	void SetMasterVolumeValue(int32 InValue) { MasterVolumeValue = InValue; }
	int32 GetMasterVolumeValue() const { return MasterVolumeValue; }

	// 게임이 실행될 때 GameUserSettings를 초기화하고 적용
	void InitializeGameUserSettings(const UWorld* World);

	// UAuraGameUserSettings의 Custom Setting과 UGameUserSettings의 Setting 적용
	void ApplyCustomSettings(bool bCheckForCommandLineOverrides, const UWorld* World);
	
	// Graphic Quality 자동 설정 및 적용
	void SetGraphicOptionByAutoDetect();

	// Changes all scalability settings at once based on a single overall quality level
	// ResolutionQuality는 무시
	void SetOverallScalabilityLevelIgnoringResolutionQuality(int32 Value);
	
	// Returns the overall scalability level (can return -1 if the settings are custom)
	// ResolutionQuality는 무시
	int32 GetOverallScalabilityLevelIgnoringResolutionQuality() const;

private:
	// 게임이 처음 실행됐는지 여부
	UPROPERTY(config)
	uint8 bFirstRun : 1;
	
	// 화면 밝기 옵션
	// 0 ~ 10 => -2 ~ 2
	UPROPERTY(config)
	int32 BrightnessValue;

	// 전체 음량 옵션
	// 0 ~ 10 => 0 ~ 1
	UPROPERTY(config)
	int32 MasterVolumeValue;
};
