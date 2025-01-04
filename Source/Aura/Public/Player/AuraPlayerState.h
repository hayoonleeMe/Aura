// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ScalableFloat.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

// AuraPlayerState에서 관리하는 Stat이 변경될 때 실행되는 델레게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStatChangedSignature, int32 /* Value */);

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Begin IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	/* End IAbilitySystemInterface */
	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }
	FORCEINLINE int32 GetSpellPoints() const { return SpellPoints; }

	// AttributePoints를 1 증가
	void IncrementAttributePoints() { AddToAttributePoints(1); }
	
	// AttributePoints를 1 감소
	void DecrementAttributePoints() { AddToAttributePoints(-1); }
	
	// AttributePoints 값을 업데이트하고 델레게이트 Broadcast
	void AddToAttributePoints(int32 InValue);

	// SpellPoints를 1 증가
	void IncrementSpellPoints() { AddToSpellPoints(1); }
	
	// SpellPoints를 1 감소
	void DecrementSpellPoints() { AddToSpellPoints(-1); }
	
	// SpellPoints 값을 업데이트하고 델레게이트 Broadcast
	void AddToSpellPoints(int32 InValue);

	// Level에 도달하는 데 필요한 XP 값을 반환
	int32 GetLevelUpXPRequirement(int32 Level) const;
	
	// Level에 도달했을 때 얻을 수 있는 Attribute Points 값을 반환
	int32 GetLevelUpAttributePointsAward(int32 Level) const;
	
	// Level에 도달했을 때 얻을 수 있는 Spell Points 값을 반환
	int32 GetLevelUpSpellPointsAward(int32 Level) const;

	FOnStatChangedSignature OnAttributePointsChangedDelegate;
	FOnStatChangedSignature OnSpellPointsChangedDelegate;
	
private:
	// ============================================================================
	// GAS
	// ============================================================================

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	// ============================================================================
	// Combat
	// ============================================================================

	// Attribute를 Upgrade하는 데 사용하는 Point
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoints)
	int32 AttributePoints;

	UFUNCTION()
	void OnRep_AttributePoints() const;

	// Spell을 Upgrade하는 데 사용하는 Point
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_SpellPoints)
	int32 SpellPoints;

	UFUNCTION()
	void OnRep_SpellPoints() const;

	// LevelUp에 필요한 XP 값을 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
	FScalableFloat XPRequirementCurve;

	// LevelUp 시 얻을 수 있는 Attribute Points 값을 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
	FScalableFloat AttributePointsAwardCurve;

	// LevelUp 시 얻을 수 있는 Spell Points 값을 저장하는 Curve
	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
	FScalableFloat SpellPointsAwardCurve;
};
