// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Character/BaseCharacter.h"
#include "Interaction/TargetInterface.h"
#include "Types/EnemyClassType.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UNiagaraComponent;
class AAuraAIController;
class UBehaviorTree;
class UWidgetComponent;
class UEnemyClassConfig;

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public ABaseCharacter, public ITargetInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();
	virtual void PossessedBy(AController* NewController) override;

	// ============================================================================
	// Overlay Widget Controller
	// ============================================================================

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChangedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChangedDelegate;
	
	/* Begin CombatInterface */
	virtual void SetCombatTarget(AActor* InCombatTarget) override { CombatTarget = InCombatTarget; }
	virtual TWeakObjectPtr<AActor> GetCombatTarget() const override { return CombatTarget; }
	virtual void GetAttackCheckRange(float& OutRadius, float& OutHalfHeight) const override;
	virtual void Die() override;
	virtual int32 GetXPReward() override;
	/* End CombatInterface */

	/* Begin TargetInterface */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/* End TargetInterface */

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeAttributes() override;

	virtual void HandleDeathLocally() override;
	
	// ============================================================================
	// Combat
	// ============================================================================

	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
	EEnemyClassType EnemyClassType;

	// Abilities.HitReact Tag가 추가, 제거될 때 호출됨
	// Count=0: Removed, Count=1: Added
	void OnHitReactTagChanged(const FGameplayTag Tag, int32 Count) const;

	// Enemy AI가 대상과의 거리를 기준으로 공격 가능 여부를 판단할 때 사용하는 거리 기준값
	UPROPERTY(EditAnywhere, Category="Aura|Combat")
	float AttackEffectiveRange;
	
	// 공격을 체크하는 Capsule의 Radius
	UPROPERTY(EditAnywhere, Category="Aura|Combat")
	float AttackRangeRadius;
	
	// 공격을 체크하는 Capsule의 Half Height
	UPROPERTY(EditAnywhere, Category="Aura|Combat")
	float AttackRangeHalfHeight;

	// AI Enemy의 공격 대상
	// Set in BTTask
	UPROPERTY()
	TWeakObjectPtr<AActor> CombatTarget;

	// ============================================================================
	// Health Bar
	// ============================================================================

	// set widget in blueprint
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> HealthBarComponent;

	// HealthBar 업데이트를 위한 초기화 수행
	void InitializeForHealthBar();

	// ============================================================================
	// Dead
	// ============================================================================

	// Enemy가 죽을 때, Destroy 될 때까지의 시간
	UPROPERTY(EditAnywhere, Category="Aura|Dead")
	float DeadLifeSpan;

	// ============================================================================
	// Dissolve
	// ============================================================================

	void Dissolve();
	
	UPROPERTY(EditAnywhere, Category="Aura|Dissolve")
	TObjectPtr<UMaterialInstance> MeshDissolveMaterial;

	UPROPERTY(EditAnywhere, Category="Aura|Dissolve")
	TObjectPtr<UMaterialInstance> WeaponMeshDissolveMaterial;

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* MID);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* MID);

	// ============================================================================
	// AI
	// ============================================================================

	UPROPERTY(EditAnywhere, Category="Aura|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;

	// ============================================================================
	// Reward
	// ============================================================================

	// 해당 Enemy를 죽일 때 플레이어가 얻는 XP Reward
	UPROPERTY(EditDefaultsOnly, Category="Aura|Reward")
	FScalableFloat XPRewardCurve;

private:
	// ============================================================================
	// Debuff Niagara Component
	// ============================================================================

	// Enfeeble Debuff가 활성화됐을 때 표시할 Niagara Component
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> EnfeebleDebuffComponent;

	// Debuff.Enfeeble Tag가 추가, 제거될 때 호출됨
	void OnDebuffEnfeebleTagChanged(const FGameplayTag Tag, int32 Count) const;
};
