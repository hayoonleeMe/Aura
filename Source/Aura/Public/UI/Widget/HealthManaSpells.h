// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraUserWidget.h"
#include "GameplayTagContainer.h"
#include "HealthManaSpells.generated.h"

struct FOnAttributeChangeData;
class UXPBar;
class USpellConfig;
class UEquippedSpellGlobe;
class UManaGlobe;
class UHealthGlobe;

/**
 * 
 */
UCLASS()
class AURA_API UHealthManaSpells : public UAuraUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void BroadcastInitialValues() override;
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHealthGlobe> HealthGlobe;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UManaGlobe> ManaGlobe;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobe> Globe_LMB;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobe> Globe_MMB;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobe> Globe_Q;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobe> Globe_W;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobe> Globe_E;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobe> Globe_R;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobe> Globe_Passive_1;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEquippedSpellGlobe> Globe_Passive_2;

	UPROPERTY()
	TMap<FGameplayTag, UEquippedSpellGlobe*> InputTagToSpellGlobeMap;

	UPROPERTY()
	TObjectPtr<USpellConfig> SpellConfig;

	// Spell의 장착 상태 변경을 업데이트
	void OnEquippedSpellChanged(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag);

	void UpdateEquippedSpellCooldown(bool bEquipped, const FGameplayTag& SpellTag, UEquippedSpellGlobe* SpellGlobe);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UXPBar> XPBar;
};
