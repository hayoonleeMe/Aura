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
class AURA_API AAuraCharacter : public ABaseCharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/* Begin CombatInterface */
	virtual int32 GetCharacterLevel_Implementation() override;
	/* End CombatInterface */

protected:
	virtual void InitAbilityActorInfo() override;

private:
	/*
	 *	Effects
	 */
	UPROPERTY(EditDefaultsOnly, Category="Aura|Effects")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;

	void InitializeAttributes();

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
};
