// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileAbility.h"

#include "Actor/AuraProjectile.h"
#include "Actor/PooledProjectile.h"
#include "Interface/ObjectPoolInterface.h"
#include "GameFramework/GameStateBase.h"

UAuraProjectileAbility::UAuraProjectileAbility()
{
	bOverridePitch = false;
	PitchOverride = 0.f;
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
	FRotator Rotation = (TargetLocation - CombatSocketLocation).Rotation();
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
		AuraProjectile->DamageEffectParams = MakeDamageEffectParams(nullptr);
		AuraProjectile->FinishSpawning(SpawnTransform);
	}
}

void UAuraProjectileAbility::SpawnPooledProjectile(const FVector& TargetLocation, const FVector& CombatSocketLocation)
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor))
	{
		return;
	}
	check(ProjectileClass);

	// Projectile 발사 방향 계산
	FRotator Rotation = (TargetLocation - CombatSocketLocation).Rotation();
	Rotation.Roll = 0.f;
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}
	
	// CombatSocket에서 Projectile 발사
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(CombatSocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	if (IObjectPoolInterface* ObjectPoolInterface = Cast<IObjectPoolInterface>(GetWorld() ? GetWorld()->GetGameState() : nullptr))
	{
		if (const APooledProjectile* ProjectileCDO = ProjectileClass->GetDefaultObject<APooledProjectile>())
		{
			if (APooledProjectile* PooledProjectile = ObjectPoolInterface->SpawnFromPool<APooledProjectile>(ProjectileCDO->GetPooledActorType(), SpawnTransform, false))
			{
				// Projectile로 데미지를 입히기 위해 설정
				PooledProjectile->DamageEffectParams = MakeDamageEffectParams(nullptr);
				PooledProjectile->SetInUse(true);
			}
		}
	}
}
