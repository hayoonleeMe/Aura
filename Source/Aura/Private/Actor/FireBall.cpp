// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/FireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interface/ObjectPoolInterface.h"
#include "GameFramework/GameStateBase.h"

AFireBall::AFireBall()
{
	ProjectileMovementComponent->InitialSpeed = 650.f;
	ProjectileMovementComponent->MaxSpeed = 650.f;
	
	FireEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Fire Effect Component"));
	FireEffectComponent->SetupAttachment(GetRootComponent());
	ImpactCueTag = AuraGameplayTags::GameplayCue_FireBallImpact;

	PooledActorType = EPooledActorType::FireBall;
}

void AFireBall::SetEmberBoltOptions(int32 InNumEmberBolts, float InEmberBoltDamage, const TSubclassOf<AAuraProjectile>& InEmberBoltClass)
{
	NumEmberBolts = InNumEmberBolts;
	EmberBoltDamage = InEmberBoltDamage;
	EmberBoltClass = InEmberBoltClass;
	check(EmberBoltClass);
}

void AFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	SpawnEmberBolts();
}

void AFireBall::SpawnEmberBolts() const
{
	TArray<FVector> Directions;
	const float SpreadAngle = 360.f / NumEmberBolts;
	UAuraBlueprintLibrary::GetSpreadDirections(Directions, NumEmberBolts, SpreadAngle, GetActorForwardVector());

	for (const FVector& Direction : Directions)
	{
		FRotator Rotation = Direction.Rotation();
		Rotation.Roll = 0.f;
		Rotation.Pitch = 0.f;
		
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetActorLocation());
		SpawnTransform.SetRotation(Rotation.Quaternion());

		// Object Pooling instead of spawn actor
		if (IObjectPoolInterface* ObjectPoolInterface = Cast<IObjectPoolInterface>(GetWorld() ? GetWorld()->GetGameState() : nullptr))
		{
			if (const APooledProjectile* ProjectileCDO = EmberBoltClass->GetDefaultObject<APooledProjectile>())
			{
				if (APooledProjectile* PooledProjectile = ObjectPoolInterface->SpawnFromPool<APooledProjectile>(ProjectileCDO->GetPooledActorType(), SpawnTransform, false))
				{
					// Projectile로 데미지를 입히기 위해 설정
					PooledProjectile->DamageEffectParams = DamageEffectParams;
					PooledProjectile->DamageEffectParams.BaseDamage = EmberBoltDamage;
					PooledProjectile->SetInUse(true);
				}
			}
		}	
	}
}

void AFireBall::OnSetInUse(bool bInUse)
{
	Super::OnSetInUse(bInUse);
	
	if (bInUse)
	{
		FireEffectComponent->Activate();
	}
	else
	{
		FireEffectComponent->Deactivate();
	}
}
