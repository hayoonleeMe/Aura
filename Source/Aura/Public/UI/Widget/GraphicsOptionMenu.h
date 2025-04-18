// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GraphicsOptionMenu.generated.h"

class UScrollBox;
class UAuraGameUserSettings;
class UOptionCheckBoxRow;
class UOptionButtonRow;
class UOptionSliderRow;
class UOptionComboBoxRow;

// 해당 메뉴의 옵션이 변경됐음을 알리는 델레게이트
DECLARE_MULTICAST_DELEGATE(FOnOptionChangedSignature);

/**
 * 변경되기 전 Graphics Option을 저장할 구조체
 */
struct FOriginalGraphicsOptions
{
	EWindowMode::Type WindowMode;
	FIntPoint Resolution;
	int32 Brightness;
	bool bVerticalSync;
	float FPSLimit;
	int32 Preset;
	int32 ViewDistance;
	int32 AntiAliasing;
	int32 PostProcessing;
	int32 Shadow;
	int32 GlobalIllumination;
	int32 Reflection;
	int32 Texture;
	int32 Effect;
	int32 Shading;

	FOriginalGraphicsOptions() = default;
	FOriginalGraphicsOptions(const UAuraGameUserSettings* AuraGameUserSettings);
};

/**
 * 
 */
UCLASS()
class AURA_API UGraphicsOptionMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	// 변경사항이 적용되고 수행
	void OnOptionSaved();

	// 변경사항 되돌리기
	void RevertChanges();

	// 아직 저장되지 않은 변경된 옵션이 있는지 반환
	bool HasOptionChanged() const;

	FOnOptionChangedSignature OnOptionChangedDelegate;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_WindowMode;
	
	UFUNCTION()
	void OnWindowModeOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_Resolution;

	UFUNCTION()
	void OnResolutionOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionSliderRow> Row_Brightness;

	UFUNCTION()
	void OnBrightnessOptionChanged(float Value);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionCheckBoxRow> Row_VerticalSync;

	UFUNCTION()
	void OnVerticalSyncOptionChanged(bool bIsChecked);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_FPSLimit;

	UFUNCTION()
	void OnFPSLimitOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionButtonRow> Row_AutoDetect;

	UFUNCTION()
	void OnAutoDetectButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_Preset;
	
	UFUNCTION()
	void OnPresetOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType);

	// 다른 Graphic Quality Option에 따라 Preset Option을 업데이트한다.
	void UpdatePresetOption() const;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_ViewDistance;

	UFUNCTION()
	void OnViewDistanceOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_AntiAliasing;

	UFUNCTION()
	void OnAntiAliasingOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_PostProcessing;

	UFUNCTION()
	void OnPostProcessingOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_Shadows;
	
	UFUNCTION()
	void OnShadowOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_GlobalIllumination;

	UFUNCTION()
    void OnGlobalIlluminationOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_Reflections;
	
	UFUNCTION()
	void OnReflectionOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_Textures;

	UFUNCTION()
	void OnTextureOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_Effects;

	UFUNCTION()
	void OnEffectOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_Shading;

	UFUNCTION()
	void OnShadingOptionChanged(FName SelectedItem, ESelectInfo::Type SelectionType);

private:
	UPROPERTY()
	TObjectPtr<UAuraGameUserSettings> AuraGameUserSettings;

	FOriginalGraphicsOptions OriginalGraphicsOptions;
	
	// 각 옵션이 변경됐는지 여부
	uint8 bWindowModeChanged:1;
	uint8 bResolutionChanged:1;
	uint8 bBrightnessChanged:1;
	uint8 bVerticalSyncChanged:1;
	uint8 bFPSLimitChanged:1;
	uint8 bPresetChanged:1;
	uint8 bViewDistanceChanged:1;
	uint8 bAntiAliasingChanged:1;
	uint8 bPostProcessingChanged:1;
	uint8 bShadowChanged:1;
	uint8 bGlobalIlluminationChanged:1;
	uint8 bReflectionChanged:1;
	uint8 bTextureChanged:1;
	uint8 bEffectChanged:1;
	uint8 bShadingChanged:1;

	// Alt + Enter로 Fullscreen 모드를 변경할 때 호출됨
	// WindowMode, Resolution Option을 업데이트
	void OnToggleFullscreen(bool bFullscreen);

	static TArray<FName> GetWindowModeOptions();

	// EWindowMode WindowMode를 나타내는 FName Option을 반환한다.
	static FName MakeWindowModeOption(EWindowMode::Type WindowMode);

	// FName WindowMode에 해당하는 EWindowMode enum을 반환한다.
	static EWindowMode::Type MakeWindowModeEnum(const FName& WindowMode);
	
	TArray<FName> GetResolutionOptions() const;

	// FIntPoint Res를 나타내는 FName Option을 반환한다.
	static FName MakeResolutionOption(const FIntPoint& Res);

	// FName Res에 해당하는 FIntPoint를 반환한다.
	static FIntPoint MakeResolutionIntPoint(const FName& Res);
 
	const TMap<int32, FName> FPSLimitOptions
    {
    	{ 0, TEXT("Unlimited") },
    	{ 30, TEXT("30 FPS") },
    	{ 60, TEXT("60 FPS") },
    	{ 120, TEXT("120 FPS") }
    };

	const TMap<FName, int32> FPSLimitOptions_Reversed
	{
		{ TEXT("Unlimited"), 0 },
		{ TEXT("30 FPS"), 30 },
		{ TEXT("60 FPS"), 60 },
		{ TEXT("120 FPS"), 120 }
	};

	TArray<FName> GetFPSLimitOptions() const;

	const TMap<int32, FName> PresetOptions
	{
		{ -1, TEXT("Custom") },
		{ 0, TEXT("Low") },
		{ 1, TEXT("Medium") },
		{ 2, TEXT("High") },
		{ 3, TEXT("Epic") }
	};

	const TMap<FName, int32> PresetOptions_Reversed
	{
		{ TEXT("Custom"), -1 },
		{ TEXT("Low"), 0 },
		{ TEXT("Medium"), 1 },
		{ TEXT("High"), 2 },
		{ TEXT("Epic"), 3 }
	};

	TArray<FName> GetPresetOptions() const;

	const TMap<int32, FName> LowToEpicQualityOptions
	{
		{ 0, TEXT("Low") },
		{ 1, TEXT("Medium") },
		{ 2, TEXT("High") },
		{ 3, TEXT("Epic") }
	};

	const TMap<FName, int32> LowToEpicQualityOptions_Reversed
	{
		{ TEXT("Low"), 0 },
		{ TEXT("Medium"), 1 },
		{ TEXT("High"), 2 },
		{ TEXT("Epic"), 3 }
	};

	TArray<FName> GetLowToEpicQualityOptions() const;

	const TMap<int32, FName> OffToEpicQualityOptions
	{
		{ 0, TEXT("Off") },
		{ 1, TEXT("Medium") },
		{ 2, TEXT("High") },
		{ 3, TEXT("Epic") }
	};

	const TMap<FName, int32> OffToEpicQualityOptions_Reversed
	{
		{ TEXT("Off"), 0 },
		{ TEXT("Medium"), 1 },
		{ TEXT("High"), 2 },
		{ TEXT("Epic"), 3 }
	};

	TArray<FName> GetOffToEpicQualityOptions() const;
	
	const TMap<int32, FName> ViewDistanceQualityOptions
	{
		{ 0, TEXT("Near") },
		{ 1, TEXT("Medium") },
		{ 2, TEXT("Far") },
		{ 3, TEXT("Epic") }
	};

	const TMap<FName, int32> ViewDistanceQualityOptions_Reversed
	{
	    { TEXT("Near"), 0 },
		{ TEXT("Medium"), 1 },
		{ TEXT("Far"), 2 },
		{ TEXT("Epic"), 3 }
	};

	TArray<FName> GetViewDistanceQualityOptions() const;
};
