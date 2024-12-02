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

	/*
	 *	Overlay Widget Controller
	 */
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChangedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChangedDelegate;
	
	/* Begin CombatInterface */
	virtual int32 GetCharacterLevel_Implementation() override { return Level; }
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
};
