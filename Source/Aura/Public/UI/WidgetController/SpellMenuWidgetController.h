// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "SpellMenuWidgetController.generated.h"

class USpellConfig;
struct FSpellInfo;

// AuraPlayerState의 SpellPoints 값이 변경되면 그 값을 전달하는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellPointsChangedSignature, int32, Value);

// Spell Tag를 가지는 Spell Globe의 선택 상태 변경을 전달하는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSelectSpellGlobeSignature, bool, bSelected, const FGameplayTag&, SpellTag, bool, bPlayDeselectSound);

// Spell이 Unlock 된 이후로, Spell의 변경된 정보를 전달하는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellChangedSignature, const FSpellInfo&, SpellInfo);

// Spell의 장착 상태 변경을 전달하는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEquippedSpellChangedSignature, bool, bEquipped, const FGameplayTag&, InputTag, const FSpellInfo&, SpellInfo);

// 서버의 ASC에서 ActivatableAbilities가 변경되어 클라이언트에서 OnRep_ActivateAbilities이 호출될 때를 전달하는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpellGivenSignature);

/**
 * Spell Menu에서 사용할 Widget Controller
 */
UCLASS()
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FOnSpellPointsChangedSignature OnSpellPointsChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FSelectSpellGlobeSignature SelectSpellGlobeDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnSpellChangedSignature OnSpellChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnEquippedSpellChangedSignature OnEquippedSpellChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnSpellGivenSignature OnSpellGivenDelegate;

	// 현재 선택한 Spell Globe가 있는지 반환
	UFUNCTION(BlueprintCallable)
	bool IsValidSelectedSpellTag() const { return SelectedSpellTag.IsValid(); }

	// 현재 선택한 SpellGlobe의 Spell이 Offensive Spell인지를 반환
	// true면 Offensive Spell, false면 Passive Spell
	UFUNCTION(BlueprintCallable)
	bool IsSelectedSpellOffensive() const;

	// 현재 선택한 SpellGlobe의 Spell이 Unlock 됐는 지를 반환  
	UFUNCTION(BlueprintCallable)
	bool IsSelectedSpellUnlocked() const;

	UFUNCTION(BlueprintCallable)
	bool CanSpendPoint() const;

	// Spell Menu에서 특정 Spell Globe를 선택
	// SelectedSpellTag를 업데이트하고, SelectSpellGlobeDelegate 실행
	UFUNCTION(BlueprintCallable)
	void SelectSpellGlobe(const FGameplayTag& SpellTag);

	// 현재 선택된 Spell Globe를 선택 해제한다.
	UFUNCTION(BlueprintCallable)
	void DeselectSpellGlobe();

	// Spell Globe를 선택하고 Spend Point Button을 클릭해 해당 Spell을 Unlock하거나 Upgrade한다.
	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();

	// 선택한 Spell을 Input에 장착
	UFUNCTION(BlueprintCallable)
	void HandleEquipSpell(const FGameplayTag& InputTag);

	// EquippedSpellRow Widget의 Globe를 선택하기를 기다리는 상태를 나타냄
	UPROPERTY(BlueprintReadWrite)
	uint8 bWaitSelectGlobe : 1;

private:
	void UpdateSpellPoints(int32 SpellPoints);
	
	// 현재 Spell Menu에서 선택 중인 Spell Globe의 Spell Tag 캐싱
	UPROPERTY()
	FGameplayTag SelectedSpellTag;

	// 현재 Spell Point를 가지고 있는지를 나타냄
	UPROPERTY()
	uint8 bHasSpellPoints : 1;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<USpellConfig> SpellConfig;
};
