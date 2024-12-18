// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
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

	FORCEINLINE int32 GetCharacterLevel() const { return Level; }
	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }
	FORCEINLINE int32 GetSpellPoints() const { return SpellPoints; }

	// AttributePoints를 1 증가
	void IncrementAttributePoints() { AddToAttributePoints(1); }
	// AttributePoints를 1 감소
	void DecrementAttributePoints() { AddToAttributePoints(-1); }

	// SpellPoints를 1 증가
	void IncrementSpellPoints() { AddToSpellPoints(1); }
	// SpellPoints를 1 감소
	void DecrementSpellPoints() { AddToSpellPoints(-1); }

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

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	// Attribute를 Upgrade하는 데 사용하는 Point
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoints)
	int32 AttributePoints;

	UFUNCTION()
	void OnRep_AttributePoints() const;

	// AttributePoints 값을 업데이트하고 델레게이트 Broadcast
	void AddToAttributePoints(int32 InValue);

	// Spell을 Upgrade하는 데 사용하는 Point
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_SpellPoints)
	int32 SpellPoints;

	UFUNCTION()
	void OnRep_SpellPoints() const;

	// SpellPoints 값을 업데이트하고 델레게이트 Broadcast
	void AddToSpellPoints(int32 InValue);
};
