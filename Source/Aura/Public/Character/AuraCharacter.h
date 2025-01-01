// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public ABaseCharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/* Begin CombatInterface */
	virtual int32 GetCharacterLevel() override;
	/* End CombatInterface */

	/* Begin PlayerInterface */
	virtual int32 GetAttributePoints() const override;
	virtual void IncrementAttributePoints() override;
	virtual void DecrementAttributePoints() override;
	virtual int32 GetSpellPoints() const override;
	virtual void IncrementSpellPoints() override;
	virtual void DecrementSpellPoints() override;
	virtual void IndicateDamage(float Damage) const override;
	/* End PlayerInterface */
	
protected:
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeAttributes() override;

	virtual void HandleDeathLocally() override;

private:
	// ============================================================================
	// Effects
	// ============================================================================

	UPROPERTY(EditDefaultsOnly, Category="Aura|Effects")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	// ============================================================================
	// Camera
	// ============================================================================

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;
};
