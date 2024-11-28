// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraDamageAbility.generated.h"

struct FDamageEffectParams;

/**
 * Damage를 입히는 GameplayAbility
 */
UCLASS()
class AURA_API UAuraDamageAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MakeDamageEffectParams(FDamageEffectParams& OutParams, AActor* TargetActor) const;

	// DamageCurve에서 Level에 따른 Damage Value를 반환한다.
	float GetDamageByLevel() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage")
	FGameplayTag DamageTypeTag;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage")
	FScalableFloat DamageCurve;
};
