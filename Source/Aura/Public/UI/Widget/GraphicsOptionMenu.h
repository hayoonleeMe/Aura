// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GraphicsOptionMenu.generated.h"

class UOptionCheckBoxRow;
class UOptionButtonRow;
class UOptionSliderRow;
class UOptionComboBoxRow;

/**
 * 
 */
UCLASS()
class AURA_API UGraphicsOptionMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_WindowMode;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_Resolution;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionSliderRow> Row_Brightness;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionCheckBoxRow> Row_VerticalSync;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_FPSLimit;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionButtonRow> Row_AutoDetect;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_Preset;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_ViewDistance;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_AntiAliasing;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_PostProcessing;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_Shadows;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_GlobalIllumination;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_Reflections;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_Textures;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_Effects;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionComboBoxRow> Row_Shading;
};
