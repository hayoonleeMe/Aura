// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/SoundOptionMenu.h"

#include "Components/Slider.h"
#include "GameUserSettings/AuraGameUserSettings.h"
#include "UI/Widget/OptionSliderRow.h"

FOriginalSoundOptions::FOriginalSoundOptions(const UAuraGameUserSettings* AuraGameUserSettings)
{
	MasterVolume = AuraGameUserSettings->GetMasterVolumeValue();
}

void USoundOptionMenu::NativeConstruct()
{
	Super::NativeConstruct();

	bMasterVolumeChanged = false;
	
	AuraGameUserSettings = CastChecked<UAuraGameUserSettings>(GEngine ? GEngine->GetGameUserSettings() : nullptr);
	OriginalSoundOptions = FOriginalSoundOptions(AuraGameUserSettings);

	Row_MasterVolume->InitializeSliderValue(AuraGameUserSettings->GetMasterVolumeValue());
	Row_MasterVolume->Slider->OnValueChanged.AddDynamic(this, &ThisClass::OnMasterVolumeOptionChanged);
}

void USoundOptionMenu::NativeDestruct()
{
	Super::NativeDestruct();

	// 적용되지 않은 옵션을 원래대로 되돌림
	if (bMasterVolumeChanged)
	{
		AuraGameUserSettings->SetMasterVolumeValue(OriginalSoundOptions.MasterVolume);
	}
}

void USoundOptionMenu::OnOptionSaved()
{
	bMasterVolumeChanged = false;

	// 저장했으므로 다시 업데이트
	OriginalSoundOptions = FOriginalSoundOptions(AuraGameUserSettings);
}

bool USoundOptionMenu::HasOptionChanged() const
{
	return bMasterVolumeChanged;
}

void USoundOptionMenu::OnMasterVolumeOptionChanged(float Value)
{
	const int32 NewMasterVolume = int32(Value);
	bMasterVolumeChanged = OriginalSoundOptions.MasterVolume != NewMasterVolume;
	AuraGameUserSettings->SetMasterVolumeValue(NewMasterVolume);
	OnOptionChangedDelegate.Broadcast();
}
