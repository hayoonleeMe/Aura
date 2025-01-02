// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

public:
	// ============================================================================
	// Attribute Points
	// ============================================================================
	
	// PlayerState의 Attribute Points 반환
	virtual int32 GetAttributePoints() const { return 0; }
	
	// PlayerState의 Attribute Points를 1 증가
	virtual void IncrementAttributePoints() {}

	// PlayerState의 Attribute Points를 1 감소
	virtual void DecrementAttributePoints() {}

	// ============================================================================
	// Spell Points
	// ============================================================================

	// PlayerState의 Spell Points 반환
	virtual int32 GetSpellPoints() const { return 0; }

	// PlayerState의 SpellPoints를 1 증가
	virtual void IncrementSpellPoints() {}

	// PlayerState의 SpellPoints를 1 감소
	virtual void DecrementSpellPoints() {}

	// ============================================================================
	// Combat
	// ============================================================================

	virtual void IndicateDamage(float Damage, const FVector_NetQuantize& TargetLocation) const {}
};
