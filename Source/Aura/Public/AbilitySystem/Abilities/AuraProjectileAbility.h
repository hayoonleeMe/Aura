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
	
	// 하나의 Projectile을 발사
	virtual void SpawnProjectile(const FVector& TargetLocation);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Aura|Projectile")
	TSubclassOf<AAuraProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Projectile")
	int32 NumProjectiles;
};
