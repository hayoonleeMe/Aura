// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/GraphicsOptionMenu.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxKey.h"
#include "Components/Slider.h"
#include "GameUserSettings/AuraGameUserSettings.h"
#include "UI/Widget/OptionButtonRow.h"
#include "UI/Widget/OptionCheckBoxRow.h"
#include "UI/Widget/OptionComboBoxRow.h"
#include "UI/Widget/OptionSliderRow.h"
#include "UI/Widget/SquareButton.h"

FOriginalGraphicsOptions::FOriginalGraphicsOptions(const UAuraGameUserSettings* AuraGameUserSettings)
{
	check(AuraGameUserSettings);
	
	WindowMode = AuraGameUserSettings->GetFullscreenMode();
	Resolution = AuraGameUserSettings->GetScreenResolution();
	Brightness = AuraGameUserSettings->GetBrightnessValue();
	bVerticalSync = AuraGameUserSettings->IsVSyncEnabled();
	FPSLimit = AuraGameUserSettings->GetFrameRateLimit();
	Preset = AuraGameUserSettings->GetOverallScalabilityLevelIgnoringResolutionQuality();
	ViewDistance = AuraGameUserSettings->GetViewDistanceQuality();
	AntiAliasing = AuraGameUserSettings->GetAntiAliasingQuality();
	PostProcessing = AuraGameUserSettings->GetPostProcessingQuality();
	Shadow = AuraGameUserSettings->GetShadowQuality();
	GlobalIllumination = AuraGameUserSettings->GetGlobalIlluminationQuality();
	Reflection = AuraGameUserSettings->GetReflectionQuality();
	Texture = AuraGameUserSettings->GetTextureQuality();
	Effect = AuraGameUserSettings->GetVisualEffectQuality();
	Shading = AuraGameUserSettings->GetShadingQuality();
}

void UGraphicsOptionMenu::NativeConstruct()
{
	Super::NativeConstruct();

	bWindowModeChanged = bResolutionChanged = bBrightnessChanged = bVerticalSyncChanged = bFPSLimitChanged = bPresetChanged = bViewDistanceChanged = bAntiAliasingChanged = bPostProcessingChanged = bShadowChanged = bGlobalIlluminationChanged = bReflectionChanged = bTextureChanged = bEffectChanged = bShadingChanged = false;
	
	AuraGameUserSettings = CastChecked<UAuraGameUserSettings>(GEngine ? GEngine->GetGameUserSettings() : nullptr);
	OriginalGraphicsOptions = FOriginalGraphicsOptions(AuraGameUserSettings);

	// Update ComboBox Option
	Row_WindowMode->SetComboBoxOptions(GetWindowModeOptions(), WindowModeOptions[AuraGameUserSettings->GetFullscreenMode()]);
	Row_Resolution->SetComboBoxOptions(GetResolutionOptions(), ResolutionOptions[AuraGameUserSettings->GetScreenResolution()]);
	Row_Brightness->InitializeSliderValue(AuraGameUserSettings->GetBrightnessValue());
	Row_VerticalSync->bChecked = AuraGameUserSettings->IsVSyncEnabled();
	Row_FPSLimit->SetComboBoxOptions(GetFPSLimitOptions(), FPSLimitOptions[int32(AuraGameUserSettings->GetFrameRateLimit())]);
	UpdateQualityOptionsComboBox();

	Row_WindowMode->ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnWindowModeOptionChanged);
	Row_Resolution->ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnResolutionOptionChanged);
	Row_Brightness->Slider->OnValueChanged.AddDynamic(this, &ThisClass::OnBrightnessOptionChanged);
	Row_VerticalSync->CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnVerticalSyncOptionChanged);
	Row_FPSLimit->ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnFPSLimitOptionChanged);
	Row_AutoDetect->Button_Option->InternalButton->OnClicked.AddDynamic(this, &ThisClass::OnAutoDetectButtonClicked);
	Row_Preset->ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnPresetOptionChanged);
	Row_ViewDistance->ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnViewDistanceOptionChanged);
	Row_AntiAliasing->ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnAntiAliasingOptionChanged);
	Row_PostProcessing->ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnPostProcessingOptionChanged);
	Row_Shadows->ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnShadowOptionChanged);
	Row_GlobalIllumination->ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnGlobalIlluminationOptionChanged);
	Row_Reflections->ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnReflectionOptionChanged);
	Row_Textures->ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnTextureOptionChanged);
	Row_Effects->ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnEffectOptionChanged);
	Row_Shading->ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnShadingOptionChanged);
}

void UGraphicsOptionMenu::NativeDestruct()
{
	Super::NativeDestruct();

	// 적용되지 않은 옵션을 원래대로 되돌림
	if(bWindowModeChanged)
	{
		AuraGameUserSettings->SetFullscreenMode(OriginalGraphicsOptions.WindowMode);
	}
	if (bResolutionChanged)
	{
		AuraGameUserSettings->SetScreenResolution(OriginalGraphicsOptions.Resolution);
	}
	if (bBrightnessChanged)
	{
		AuraGameUserSettings->SetBrightnessValue(OriginalGraphicsOptions.Brightness);
	}
	if (bVerticalSyncChanged)
	{
		AuraGameUserSettings->SetVSyncEnabled(OriginalGraphicsOptions.bVerticalSync);
	}
	if (bFPSLimitChanged)
	{
		AuraGameUserSettings->SetFrameRateLimit(OriginalGraphicsOptions.FPSLimit);
	}
	if (bPresetChanged && OriginalGraphicsOptions.Preset >= 0)
	{
		AuraGameUserSettings->SetOverallScalabilityLevelIgnoringResolutionQuality(OriginalGraphicsOptions.Preset);
		bViewDistanceChanged = bAntiAliasingChanged = bPostProcessingChanged = bShadingChanged = bGlobalIlluminationChanged = bReflectionChanged = bTextureChanged = bEffectChanged = bShadingChanged = false;
	}
	if (bViewDistanceChanged)
	{
		AuraGameUserSettings->SetViewDistanceQuality(OriginalGraphicsOptions.ViewDistance);
	}
	if (bAntiAliasingChanged)
	{
		AuraGameUserSettings->SetAntiAliasingQuality(OriginalGraphicsOptions.AntiAliasing);
	}
	if (bPostProcessingChanged)
	{
		AuraGameUserSettings->SetPostProcessingQuality(OriginalGraphicsOptions.PostProcessing);
	}
	if (bShadowChanged)
	{
		AuraGameUserSettings->SetShadowQuality(OriginalGraphicsOptions.Shadow);
	}
	if (bGlobalIlluminationChanged)
	{
		AuraGameUserSettings->SetGlobalIlluminationQuality(OriginalGraphicsOptions.GlobalIllumination);
	}
	if (bReflectionChanged)
	{
		AuraGameUserSettings->SetReflectionQuality(OriginalGraphicsOptions.Reflection);
	}
	if (bTextureChanged)
	{
		AuraGameUserSettings->SetTextureQuality(OriginalGraphicsOptions.Texture);
	}
	if (bEffectChanged)
	{
		AuraGameUserSettings->SetVisualEffectQuality(OriginalGraphicsOptions.Effect);
	}
	if (bShadingChanged)
	{
		AuraGameUserSettings->SetShadingQuality(OriginalGraphicsOptions.Shading);
	}
}

void UGraphicsOptionMenu::OnOptionSaved()
{
	bWindowModeChanged = bResolutionChanged = bBrightnessChanged = bVerticalSyncChanged = bFPSLimitChanged = bPresetChanged = bViewDistanceChanged = bAntiAliasingChanged = bPostProcessingChanged = bShadowChanged = bGlobalIlluminationChanged = bReflectionChanged = bTextureChanged = bEffectChanged = bShadingChanged = false;

	// 저장했으므로 다시 업데이트
	OriginalGraphicsOptions = FOriginalGraphicsOptions(AuraGameUserSettings);
}

bool UGraphicsOptionMenu::HasOptionChanged() const
{
	return bWindowModeChanged || bResolutionChanged || bBrightnessChanged || bVerticalSyncChanged || bFPSLimitChanged || bPresetChanged || bViewDistanceChanged || bAntiAliasingChanged || bPostProcessingChanged || bShadowChanged || bGlobalIlluminationChanged || bReflectionChanged || bTextureChanged || bEffectChanged || bShadingChanged;
}

void UGraphicsOptionMenu::UpdateQualityOptionsComboBox() const
{
	Row_Preset->SetComboBoxOptions(GetPresetOptions(), PresetOptions[AuraGameUserSettings->GetOverallScalabilityLevelIgnoringResolutionQuality()]);
	Row_ViewDistance->SetComboBoxOptions(GetViewDistanceQualityOptions(), ViewDistanceQualityOptions[AuraGameUserSettings->GetViewDistanceQuality()]);
	Row_AntiAliasing->SetComboBoxOptions(GetOffToEpicQualityOptions(), OffToEpicQualityOptions[AuraGameUserSettings->GetAntiAliasingQuality()]);
	Row_PostProcessing->SetComboBoxOptions(GetLowToEpicQualityOptions(), LowToEpicQualityOptions[AuraGameUserSettings->GetPostProcessingQuality()]);
	Row_Shadows->SetComboBoxOptions(GetOffToEpicQualityOptions(), OffToEpicQualityOptions[AuraGameUserSettings->GetShadowQuality()]);
	Row_GlobalIllumination->SetComboBoxOptions(GetLowToEpicQualityOptions(), LowToEpicQualityOptions[AuraGameUserSettings->GetGlobalIlluminationQuality()]);
	Row_Reflections->SetComboBoxOptions(GetLowToEpicQualityOptions(), LowToEpicQualityOptions[AuraGameUserSettings->GetReflectionQuality()]);
	Row_Textures->SetComboBoxOptions(GetLowToEpicQualityOptions(), LowToEpicQualityOptions[AuraGameUserSettings->GetTextureQuality()]);
	Row_Effects->SetComboBoxOptions(GetLowToEpicQualityOptions(), LowToEpicQualityOptions[AuraGameUserSettings->GetVisualEffectQuality()]);
	Row_Shading->SetComboBoxOptions(GetLowToEpicQualityOptions(), LowToEpicQualityOptions[AuraGameUserSettings->GetShadingQuality()]);
}

void UGraphicsOptionMenu::OnWindowModeOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType)
{
	const EWindowMode::Type NewWindowMode = WindowModeOptions_Reversed[SelectedItem];
	bWindowModeChanged = OriginalGraphicsOptions.WindowMode != NewWindowMode;
	AuraGameUserSettings->SetFullscreenMode(NewWindowMode);
	OnOptionChangedDelegate.Broadcast();
}

void UGraphicsOptionMenu::OnResolutionOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType)
{
	const FIntPoint NewResolution = ResolutionOptions_Reversed[SelectedItem];
	bResolutionChanged = OriginalGraphicsOptions.Resolution != NewResolution;
	AuraGameUserSettings->SetScreenResolution(NewResolution);
	OnOptionChangedDelegate.Broadcast();
}

void UGraphicsOptionMenu::OnBrightnessOptionChanged(float Value)
{
	const int32 NewBrightness = int32(Value);
	bBrightnessChanged = OriginalGraphicsOptions.Brightness != NewBrightness;
	AuraGameUserSettings->SetBrightnessValue(NewBrightness);	
	OnOptionChangedDelegate.Broadcast();
}

void UGraphicsOptionMenu::OnVerticalSyncOptionChanged(bool bIsChecked)
{
	bVerticalSyncChanged = OriginalGraphicsOptions.bVerticalSync != bIsChecked;
	AuraGameUserSettings->SetVSyncEnabled(bIsChecked);
	OnOptionChangedDelegate.Broadcast();
}

void UGraphicsOptionMenu::OnFPSLimitOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType)
{
	const float NewFPSLimit = FPSLimitOptions_Reversed[SelectedItem];
	bFPSLimitChanged = OriginalGraphicsOptions.FPSLimit != NewFPSLimit;
	AuraGameUserSettings->SetFrameRateLimit(NewFPSLimit);
	OnOptionChangedDelegate.Broadcast();
}

void UGraphicsOptionMenu::OnAutoDetectButtonClicked()
{
	// Store benchmark result in ScalabilityQuality
	AuraGameUserSettings->RunHardwareBenchmark();

	// ResolutionQuality는 항상 100으로 고정
	AuraGameUserSettings->SetResolutionScaleValueEx(100.f);

	UpdateQualityOptionsComboBox();
	
	bPresetChanged = OriginalGraphicsOptions.Preset != AuraGameUserSettings->GetOverallScalabilityLevelIgnoringResolutionQuality();
	bViewDistanceChanged = OriginalGraphicsOptions.ViewDistance != AuraGameUserSettings->GetViewDistanceQuality();
	bAntiAliasingChanged = OriginalGraphicsOptions.AntiAliasing != AuraGameUserSettings->GetAntiAliasingQuality();
	bPostProcessingChanged = OriginalGraphicsOptions.PostProcessing != AuraGameUserSettings->GetPostProcessingQuality();
	bShadowChanged = OriginalGraphicsOptions.Shadow != AuraGameUserSettings->GetShadowQuality();
	bGlobalIlluminationChanged = OriginalGraphicsOptions.GlobalIllumination != AuraGameUserSettings->GetGlobalIlluminationQuality();
	bReflectionChanged = OriginalGraphicsOptions.Reflection != AuraGameUserSettings->GetReflectionQuality();
	bTextureChanged = OriginalGraphicsOptions.Texture != AuraGameUserSettings->GetTextureQuality();
	bEffectChanged = OriginalGraphicsOptions.Effect != AuraGameUserSettings->GetVisualEffectQuality();
	bShadingChanged = OriginalGraphicsOptions.Shading != AuraGameUserSettings->GetShadingQuality();
	
	OnOptionChangedDelegate.Broadcast();
}

void UGraphicsOptionMenu::OnPresetOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType)
{
	const int32 NewPreset = PresetOptions_Reversed[SelectedItem];
	bPresetChanged = OriginalGraphicsOptions.Preset != NewPreset;
	OnOptionChangedDelegate.Broadcast();

	// 다른 퀄리티 옵션을 Preset으로 설정 (Custom 제외)
	if (NewPreset >= 0)
	{
		AuraGameUserSettings->SetOverallScalabilityLevelIgnoringResolutionQuality(NewPreset);
		const FName ViewDistanceOption = ViewDistanceQualityOptions[NewPreset]; 
		const FName OffToEpicQualityOption = OffToEpicQualityOptions[NewPreset]; 
		const FName LowToEpicQualityOption = LowToEpicQualityOptions[NewPreset]; 
		Row_ViewDistance->ComboBox->SetSelectedOption(ViewDistanceOption);
		Row_AntiAliasing->ComboBox->SetSelectedOption(OffToEpicQualityOption);
		Row_PostProcessing->ComboBox->SetSelectedOption(LowToEpicQualityOption);
		Row_Shadows->ComboBox->SetSelectedOption(OffToEpicQualityOption);
		Row_GlobalIllumination->ComboBox->SetSelectedOption(LowToEpicQualityOption);
		Row_Reflections->ComboBox->SetSelectedOption(LowToEpicQualityOption);
		Row_Textures->ComboBox->SetSelectedOption(LowToEpicQualityOption);
		Row_Effects->ComboBox->SetSelectedOption(LowToEpicQualityOption);
		Row_Shading->ComboBox->SetSelectedOption(LowToEpicQualityOption);
	}
}

void UGraphicsOptionMenu::UpdatePresetOption() const
{
	const int32 NewPreset = AuraGameUserSettings->GetOverallScalabilityLevelIgnoringResolutionQuality();
	Row_Preset->ComboBox->SetSelectedOption(PresetOptions[NewPreset]);
}

void UGraphicsOptionMenu::OnViewDistanceOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType)
{
	const int32 NewViewDistance = ViewDistanceQualityOptions_Reversed[SelectedItem];
	bViewDistanceChanged = OriginalGraphicsOptions.ViewDistance != NewViewDistance;
	AuraGameUserSettings->SetViewDistanceQuality(NewViewDistance);
	OnOptionChangedDelegate.Broadcast();
	UpdatePresetOption();
}

void UGraphicsOptionMenu::OnAntiAliasingOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType)
{
	const int32 NewAntiAliasing = OffToEpicQualityOptions_Reversed[SelectedItem];
	bAntiAliasingChanged = OriginalGraphicsOptions.AntiAliasing != NewAntiAliasing;
	AuraGameUserSettings->SetAntiAliasingQuality(NewAntiAliasing);
	OnOptionChangedDelegate.Broadcast();
	UpdatePresetOption();
}

void UGraphicsOptionMenu::OnPostProcessingOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType)
{
	const int32 NewPostProcessing = LowToEpicQualityOptions_Reversed[SelectedItem];
	bPostProcessingChanged = OriginalGraphicsOptions.PostProcessing != NewPostProcessing;
	AuraGameUserSettings->SetPostProcessingQuality(NewPostProcessing);
	OnOptionChangedDelegate.Broadcast();
	UpdatePresetOption();
}

void UGraphicsOptionMenu::OnShadowOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType)
{
	const int32 NewShadow = OffToEpicQualityOptions_Reversed[SelectedItem];
	bShadowChanged = OriginalGraphicsOptions.Shadow != NewShadow;
	AuraGameUserSettings->SetShadowQuality(NewShadow);
	OnOptionChangedDelegate.Broadcast();
	UpdatePresetOption();
}

void UGraphicsOptionMenu::OnGlobalIlluminationOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType)
{
	const int32 NewGlobalIllumination = LowToEpicQualityOptions_Reversed[SelectedItem];
	bGlobalIlluminationChanged = OriginalGraphicsOptions.GlobalIllumination != NewGlobalIllumination;
	AuraGameUserSettings->SetGlobalIlluminationQuality(NewGlobalIllumination);
	OnOptionChangedDelegate.Broadcast();
	UpdatePresetOption();
}

void UGraphicsOptionMenu::OnReflectionOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType)
{
	const int32 NewReflection = LowToEpicQualityOptions_Reversed[SelectedItem];
	bReflectionChanged = OriginalGraphicsOptions.Reflection != NewReflection;
	AuraGameUserSettings->SetReflectionQuality(NewReflection);
	OnOptionChangedDelegate.Broadcast();
	UpdatePresetOption();
}

void UGraphicsOptionMenu::OnTextureOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType)
{
	const int32 NewTexture = LowToEpicQualityOptions_Reversed[SelectedItem];
	bTextureChanged = OriginalGraphicsOptions.Texture != NewTexture;
	AuraGameUserSettings->SetTextureQuality(NewTexture);
	OnOptionChangedDelegate.Broadcast();
	UpdatePresetOption();
}

void UGraphicsOptionMenu::OnEffectOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType)
{
	const int32 NewEffect = LowToEpicQualityOptions_Reversed[SelectedItem];
	bEffectChanged = OriginalGraphicsOptions.Effect != NewEffect;
	AuraGameUserSettings->SetVisualEffectQuality(NewEffect);
	OnOptionChangedDelegate.Broadcast();
	UpdatePresetOption();
}

void UGraphicsOptionMenu::OnShadingOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType)
{
	const int32 NewShading = LowToEpicQualityOptions_Reversed[SelectedItem];
	bShadingChanged = OriginalGraphicsOptions.Shading != NewShading;
	AuraGameUserSettings->SetShadingQuality(NewShading);
	OnOptionChangedDelegate.Broadcast();
	UpdatePresetOption();
}

TArray<FName> UGraphicsOptionMenu::GetWindowModeOptions() const
{
	TArray<FName> Options;
	for (const auto& Pair : WindowModeOptions)
	{
		Options.Add(Pair.Value);
	}
	return Options;
}

TArray<FName> UGraphicsOptionMenu::GetResolutionOptions() const
{
	TArray<FName> Options;
	for (const auto& Pair : ResolutionOptions)
	{
		Options.Add(Pair.Value);
	}
	return Options;
}

TArray<FName> UGraphicsOptionMenu::GetFPSLimitOptions() const
{
	TArray<FName> Options;
	for (const auto& Pair : FPSLimitOptions)
	{
		Options.Add(Pair.Value);
	}
	return Options;
}

TArray<FName> UGraphicsOptionMenu::GetPresetOptions() const
{
	TArray<FName> Options;
	for (const auto& Pair : PresetOptions)
	{
		Options.Add(Pair.Value);
	}
	return Options;
}

TArray<FName> UGraphicsOptionMenu::GetLowToEpicQualityOptions() const
{
	TArray<FName> Options;
	for (const auto& Pair : LowToEpicQualityOptions)
	{
		Options.Add(Pair.Value);
	}
	return Options;
}

TArray<FName> UGraphicsOptionMenu::GetOffToEpicQualityOptions() const
{
	TArray<FName> Options;
	for (const auto& Pair : OffToEpicQualityOptions)
	{
		Options.Add(Pair.Value);
	}
	return Options;
}

TArray<FName> UGraphicsOptionMenu::GetViewDistanceQualityOptions() const
{
	TArray<FName> Options;
	for (const auto& Pair : ViewDistanceQualityOptions)
	{
		Options.Add(Pair.Value);
	}
	return Options;
}
