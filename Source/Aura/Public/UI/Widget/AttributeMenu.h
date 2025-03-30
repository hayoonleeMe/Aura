// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraUserWidget.h"
#include "GameplayTagContainer.h"
#include "AttributeMenu.generated.h"

class UAttributeConfig;
class USquareButton;
class UAttributePointsRow;
class UTextValueRow;
class UTextValueButtonRow;

/**
 * 
 */
UCLASS()
class AURA_API UAttributeMenu : public UAuraUserWidget
{
	GENERATED_BODY()

public:
	UAttributeMenu(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UAttributePointsRow> Row_AttributePoints;

	void UpdateAttributePointsChange(int32 Value) const;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueButtonRow> Row_Strength;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueButtonRow> Row_Intelligence;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueButtonRow> Row_Resilience;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueButtonRow> Row_Vigor;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueRow> Row_Armor;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueRow> Row_ArmorPenetration;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueRow> Row_BlockChance;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueRow> Row_CriticalHitChance;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueRow> Row_CriticalHitDamage;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueRow> Row_CriticalHitResistance;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueRow> Row_HealthRegeneration;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueRow> Row_ManaRegeneration;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueRow> Row_MaxHealth;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueRow> Row_MaxMana;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueRow> Row_FireResistance;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueRow> Row_LightningResistance;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueRow> Row_ArcaneResistance;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextValueRow> Row_PhysicalResistance;

	UPROPERTY()
	TMap<FGameplayTag, UTextValueRow*> AttributeTagToRowMap;

	void UpdateAttributeValueChange(const FGameplayTag& AttributeTag, float Value);

	UPROPERTY()
	TObjectPtr<UAttributeConfig> AttributeConfig;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_Close;

	UFUNCTION()
	void OnCloseButtonClicked();
};
