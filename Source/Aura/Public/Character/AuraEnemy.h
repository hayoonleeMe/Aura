// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Types/EnemyClassType.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*
	 *	Overlay Widget Controller
	 */
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChangedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChangedDelegate;
	
	/* Begin CombatInterface */
	virtual int32 GetCharacterLevel_Implementation() override { return Level; }
	virtual void Die_Implementation() override;
	/* End CombatInterface */

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeAttributes() override;
	
private:
	/*
	 *	Combat
	 */
	UPROPERTY(EditAnywhere, Category="Aura|Combat")
	int32 Level;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
	EEnemyClassType EnemyClassType;

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
	// Retrieved from enemy's anim instance to play death animation
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Dead, Category="Aura|Dead", meta=(AllowPrivateAccess=true))
	uint8 bDead : 1;

	UFUNCTION()
	void OnRep_Dead() const;

	// Enemy가 죽을 때, Destroy 될 때까지의 시간
	UPROPERTY(EditAnywhere, Category="Aura|Dead")
	float DeadLifeSpan;

	// Local에서만 적용되는 Dead 관련 작업을 수행
	void HandleDeathLocally() const;
};
