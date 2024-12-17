// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileAbility.h"

#include "Actor/AuraProjectile.h"

UAuraProjectileAbility::UAuraProjectileAbility()
{
	NumProjectiles = 1;
	bOverridePitch = false;
	PitchOverride = 0.f;
	bUseCharacterLocationForDirection = false;
}

void UAuraProjectileAbility::SpawnProjectile(const FVector& TargetLocation, const FVector& CombatSocketLocation)
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor) || !AvatarActor->HasAuthority())	// Only Spawn in Server
	{
		return;
	}
	check(ProjectileClass);

	// Projectile 발사 방향 계산
	const FVector StartLocation = bUseCharacterLocationForDirection ? AvatarActor->GetActorLocation() : CombatSocketLocation;
	FRotator Rotation = (TargetLocation - StartLocation).Rotation();
	Rotation.Roll = 0.f;
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}
	
	// CombatSocket에서 Projectile 발사
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(CombatSocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	AActor* OwningActor = GetOwningActorFromActorInfo();
	if (AAuraProjectile* AuraProjectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, OwningActor, Cast<APawn>(OwningActor), ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
	{
		// Projectile로 데미지를 입히기 위해 설정
		MakeDamageEffectParams(AuraProjectile->DamageEffectParams, nullptr);
		AuraProjectile->FinishSpawning(SpawnTransform);
	}
}
