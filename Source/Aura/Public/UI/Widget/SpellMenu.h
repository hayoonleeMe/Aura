// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ClosableWidget.h"
#include "SpellMenu.generated.h"

class UAuraAbilitySystemComponent;
class USpellDescription;
class UEquippedSpellGlobeButton;
class UEquippedSpellRow;
struct FGameplayTag;
class USpellConfig;
class USpellGlobeButton;
class UPassiveSpellTree;
class UOffensiveSpellTree;
class UFramedValue;
class USquareButton;

/**
 * 
 */
UCLASS()
class AURA_API USpellMenu : public UClosableWidget
{
	GENERATED_BODY()

public:
	USpellMenu(const FObjectInitializer& ObjectInitializer);

	/* Begin IClosableWidgetInterface */
	virtual void CloseMenu() override;
	/* End IClosableWidgetInterface */

protected:
	virtual void NativeConstruct() override;

	void BroadcastInitialValues();
	
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraASC;

public:
	UPROPERTY()
	TObjectPtr<USpellConfig> SpellConfig;
	
	// 서버에서 Spell이 변경되어 ASC의 ActivatableAbilities 프로퍼티가 Replicated 될 때 호출된다. (Client Only)
	void OnSpellGiven();
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UFramedValue> FramedValue_SpellPoints;

	void UpdateSpellPointsChange(int32 Value);

	bool bHasSpellPoints = false;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_SpendPoint;

	UFUNCTION()
	void OnSpendPointButtonClicked();
	
	bool CanSpendPoint() const;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_Equip;

	UFUNCTION()
	void OnEquipButtonClicked();

	bool CanEquipSpell() const;

	// 상황에 따라 Button_SpendPoint와 Button_Equip을 활성화하거나 비활성화한다.
	void EnableButtons() const;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_Close;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellDescription> SpellDescription;

	UPROPERTY(meta=(BindWidget))
    TObjectPtr<USpellDescription> NextLevelSpellDescription;

	void UpdateDescription() const;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOffensiveSpellTree> OffensiveSpellTree;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UPassiveSpellTree> PassiveSpellTree;

	UPROPERTY()
	TArray<USpellGlobeButton*> SpellGlobeButtons;

	void OnSpellGlobeButtonSelected(USpellGlobeButton* SelectedSpellGlobeButton);

	UPROPERTY()
	TObjectPtr<USpellGlobeButton> CurrentSelectedSpellGlobeButton;

	// 현재 선택된 스펠이 Offensive인지 여부를 반환한다.
	// Passive 스펠인 경우 false를 반환한다.
	bool IsSelectedSpellOffensive() const;

	void UpdateSpellChange(const FGameplayTag& SpellTag);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEquippedSpellRow> EquippedSpellRow;

	UPROPERTY()
	TArray<UEquippedSpellGlobeButton*> EquippedSpellGlobeButtons;

	void OnEquippedSpellGlobeButtonSelected(const FGameplayTag& InputTag);

	void UpdateEquippedSpellChange(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> SpellEquipSound;
};
