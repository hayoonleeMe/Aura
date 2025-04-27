// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageAbility.h"
#include "AuraProjectileAbility.generated.h"

class AAuraProjectile;

/**
 * Projectile을 발사하는 Ability
 */
UCLASS()
class AURA_API UAuraProjectileAbility : public UAuraDamageAbility
{
	GENERATED_BODY()

public:
	UAuraProjectileAbility();
	
	// CombatSocketLocation에서 TargetLocation으로 하나의 Projectile을 발사
	virtual void SpawnProjectile(const FVector& TargetLocation, const FVector& CombatSocketLocation);
	
	// CombatSocketLocation에서 TargetLocation으로 하나의 Pooled Projectile을 발사
	virtual void SpawnPooledProjectile(const FVector& TargetLocation, const FVector& CombatSocketLocation);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Aura|Projectile")
	TSubclassOf<AAuraProjectile> ProjectileClass;

	// Projectile의 초기 Rotation Pitch를 Override 할 것인지
	UPROPERTY(EditDefaultsOnly, Category="Aura|Projectile")
	uint8 bOverridePitch : 1;

	// bOverridePitch가 true일 때, Projectile의 초기 Rotation Pitch 값
	UPROPERTY(EditDefaultsOnly, Category="Aura|Projectile", meta=(EditCondition="bOverridePitch"))
	float PitchOverride;
};
