// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "AuraCharacter.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

protected:
	virtual void InitAbilityActorInfo() override;

private:
	/*
	 *	Effects
	 */
	UPROPERTY(EditDefaultsOnly, Category="Aura|Effects")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;

	void InitializeAttributes();
};
