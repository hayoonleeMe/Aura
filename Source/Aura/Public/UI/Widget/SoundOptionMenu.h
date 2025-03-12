// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SoundOptionMenu.generated.h"

class UAuraGameUserSettings;
class UOptionSliderRow;

// 해당 메뉴의 옵션이 변경됐음을 알리는 델레게이트
DECLARE_MULTICAST_DELEGATE(FOnOptionChangedSignature);

/**
 * 변경되기 전 Sound Option을 저장할 구조체
 */
struct FOriginalSoundOptions
{
	int32 MasterVolume;

	FOriginalSoundOptions() = default;
	FOriginalSoundOptions(const UAuraGameUserSettings* AuraGameUserSettings);
};

/**
 * 
 */
UCLASS()
class AURA_API USoundOptionMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
public:
	// 변경사항이 적용되고 수행
	void OnOptionSaved();
	
	// 변경사항 되돌리기
	void RevertChanges();

	// 아직 저장되지 않은 변경된 옵션이 있는지 반환
	bool HasOptionChanged() const;
	
	FOnOptionChangedSignature OnOptionChangedDelegate;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOptionSliderRow> Row_MasterVolume;

	UFUNCTION()
	void OnMasterVolumeOptionChanged(float Value);

private:
	UPROPERTY()
	TObjectPtr<UAuraGameUserSettings> AuraGameUserSettings;

	FOriginalSoundOptions OriginalSoundOptions;

	// 각 옵션이 변경됐는지 여부
	uint8 bMasterVolumeChanged:1;
};
