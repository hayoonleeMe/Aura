// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Interaction/CombatInterface.h"
#include "Types/EnemyClassType.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public ABaseCharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	/* Begin CombatInterface */
	virtual int32 GetCharacterLevel_Implementation() override { return Level; }
	/* End CombatInterface */
	
private:
	/*
	 *	Combat
	 */
	UPROPERTY(EditAnywhere, Category="Aura|Combat")
	int32 Level;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Combat")
	EEnemyClassType EnemyClassType;
};
