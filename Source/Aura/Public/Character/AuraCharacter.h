// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/* Begin CombatInterface */
	virtual int32 GetCharacterLevel_Implementation() override;
	virtual void Die_Implementation() override;
	/* End CombatInterface */

protected:
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeAttributes() override;

private:
	/*
	 *	Effects
	 */
	UPROPERTY(EditDefaultsOnly, Category="Aura|Effects")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;

	/*
	 *	Abilities
	 */
	UPROPERTY(EditDefaultsOnly, Category="Aura|Ability")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	/*
	 *	Camera
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	/*
	 *	Dead
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleDeath();
};
