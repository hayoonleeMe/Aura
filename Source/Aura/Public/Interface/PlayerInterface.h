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

	// PlayerState의 AttributePoints를 Points만큼 증가
	virtual void AddToAttributePoints(int32 Points) {}
	
	// PlayerState의 Attribute Points를 1 증가
	virtual void IncrementAttributePoints() {}

	// PlayerState의 Attribute Points를 1 감소
	virtual void DecrementAttributePoints() {}

	// ============================================================================
	// Spell Points
	// ============================================================================

	// PlayerState의 Spell Points 반환
	virtual int32 GetSpellPoints() const { return 0; }

	// PlayerState의 SpellPoints를 Points만큼 증가
	virtual void AddToSpellPoints(int32 Points) {}

	// PlayerState의 SpellPoints를 1 증가
	virtual void IncrementSpellPoints() {}

	// PlayerState의 SpellPoints를 1 감소
	virtual void DecrementSpellPoints() {}

	// ============================================================================
	// Combat
	// ============================================================================

	// Damage Indicator Widget을 표시
	virtual void IndicateDamage(float Damage, bool bIsBlockedHit, bool bIsCriticalHit, const FVector_NetQuantize& TargetLocation) const {}

	// AuraPlayerController에서 Caching한 Target HitResult를 반환한다.
	virtual FHitResult GetTargetHitResult() const { return FHitResult(); }

	// Level에 도달하는 데 필요한 XP 값을 반환
	virtual int32 GetLevelUpXpRequirement(int32 Level) const { return 0; }
	
	// Level에 도달했을 때 얻을 수 있는 Attribute Points 값을 반환
	virtual int32 GetLevelUpAttributePointsAward(int32 Level) const { return 0; }
	
	// Level에 도달했을 때 얻을 수 있는 Spell Points 값을 반환
	virtual int32 GetLevelUpSpellPointsAward(int32 Level) const { return 0; }

	// Cost 때문에 어빌리티 실행에 실패했음을 UI에 표시
	virtual void IndicateAbilityActivateCostFail() {}

	// Cooldown 때문에 어빌리티 실행에 실패했음을 UI에 표시
	virtual void IndicateAbilityActivateCooldownFail() {}

	// Enemy 죽음을 알림
	virtual void NotifyEnemyDead() {}

	// ============================================================================
	// Input
	// ============================================================================

	// UI IMC를 추가하고, Ability IMC를 제거한다.
	virtual void AddUIMappingContext() const {}

	// UI IMC를 제거하고, 다시 Ability IMC를 추가한다.
	virtual void RemoveUIMappingContext() const {}
};
