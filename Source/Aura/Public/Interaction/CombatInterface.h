// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// Character가 죽을 때를 알리는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeadSignature);

/**
 * GameplayTag에 연관된 Combat 정보들을 저장하는 구조체
 */
USTRUCT(BlueprintType)
struct FTaggedCombatInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag Tag;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> AnimMontage;

	UPROPERTY(EditDefaultsOnly)
	FName CombatSocketName;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

public:
	// Character의 RoleTag 반환
	// Role.Player, Role.Enemy
	virtual FGameplayTag GetRoleTag() const { return FGameplayTag(); }

	// Anim Montage의 MotionWarping을 위한 FacingTarget 설정
	virtual void SetFacingTarget(const FVector& TargetLocation) {}

	// Character가 죽을 때 호출
	virtual void Die() {}

	// Character가 죽었는지 반환
	virtual bool IsDead() const { return false; }

	// Enemy의 AttackRangeRadius, AttackRangeHalfHeight 반환
	virtual void GetAttackCheckRange(float& OutRadius, float& OutHalfHeight) const {}

	// Character의 InTag에 해당하는 TaggedCombatInfo 반환
	virtual FTaggedCombatInfo GetTaggedCombatInfo(const FGameplayTag& InTag) const { return FTaggedCombatInfo(); }

	// Character의 Mesh, WeaponMesh에서 이름이 CombatSocketName인 Socket을 찾아 Location 반환
	// Mesh, WeaponMesh에 Socket이 없으면 ZeroVector 반환
	virtual FVector GetCombatSocketLocation(const FName& CombatSocketName) const { return FVector::ZeroVector; }

	// Character의 Mesh, WeaponMesh에서 이름이 CombatSocketName인 Socket을 찾아 Transform 반환
	// Mesh, WeaponMesh에 Socket이 없으면 Identity 반환
	virtual FTransform GetCombatSocketTransform(const FName& CombatSocketName) const { return FTransform::Identity; }

	// AI Enemy의 공격 대상 저장
	virtual void SetCombatTarget(AActor* InCombatTarget) {}

	// AI Enemy의 공격 대상을 반환
	virtual TWeakObjectPtr<AActor> GetCombatTarget() const { return nullptr; }

	virtual FOnCharacterDeadSignature* GetOnCharacterDeadDelegate() { return nullptr; }

	// 해당 Enemy를 죽일 때 얻을 수 있는 XP 값을 반환 
	virtual int32 GetXPReward() { return 0; }
};
