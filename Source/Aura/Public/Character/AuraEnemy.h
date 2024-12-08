// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Types/EnemyClassType.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class AAuraAIController;
class UBehaviorTree;
class UWidgetComponent;
class UEnemyClassConfig;

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AAuraEnemy();
	virtual void PossessedBy(AController* NewController) override;

	/*
	 *	Overlay Widget Controller
	 */
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChangedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChangedDelegate;
	
	/* Begin CombatInterface */
	virtual int32 GetCharacterLevel_Implementation() override { return Level; }
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override { CombatTarget = InCombatTarget; }
	virtual TWeakObjectPtr<AActor> GetCombatTarget() const override { return CombatTarget; }
	virtual void GetAttackCheckRange_Implementation(float& OutRadius, float& OutHalfHeight) const override;
	virtual void Die() override;
	/* End CombatInterface */

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeAttributes() override;

	virtual void HandleDeathLocally() const override;
	
private:
	/*
	 *	Combat
	 */
	UPROPERTY(EditAnywhere, Category="Aura|Combat")
	int32 Level;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
	EEnemyClassType EnemyClassType;

	// Abilities.HitReact Tag가 추가, 제거 될 때 호출됨
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

	/*
	 *	Health Bar
	 */
	// set widget in blueprint
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> HealthBarComponent;

	// HealthBar 업데이트를 위한 초기화 수행
	void InitializeForHealthBar();

	/*
	 * Dead
	 */
	// Enemy가 죽을 때, Destroy 될 때까지의 시간
	UPROPERTY(EditAnywhere, Category="Aura|Dead")
	float DeadLifeSpan;

	/*
	 *	AI
	 */
	UPROPERTY(EditAnywhere, Category="Aura|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;
};
