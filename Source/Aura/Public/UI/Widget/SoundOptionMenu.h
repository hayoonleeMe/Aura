// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SoundOptionMenu.generated.h"

class UOptionSliderRow;

/**
 * 
 */
UCLASS()
class AURA_API USoundOptionMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionSliderRow> Row_MasterVolume;
};
