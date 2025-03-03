// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraUserWidget.h"
#include "EquippedSpellRow.generated.h"

class UImage;
struct FGameplayTag;
class UEquippedSpellGlobeButton;

/**
 * 
 */
UCLASS()
class AURA_API UEquippedSpellRow : public UAuraUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobeButton> GlobeButton_LMB;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobeButton> GlobeButton_MMB;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobeButton> GlobeButton_Q;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobeButton> GlobeButton_W;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobeButton> GlobeButton_E;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobeButton> GlobeButton_R;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobeButton> GlobeButton_Passive_1;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobeButton> GlobeButton_Passive_2;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_OffensiveSelection;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UImage> Image_PassiveSelection;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SelectOffensiveAnimation;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SelectPassiveAnimation;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> SpellEquipSound;

	bool bWaitSelectGlobe = false;

	void WaitSelectGlobe(bool bWaitOffensive);
	void FinishSpellEquip(bool bPlayEquipSound);
};
