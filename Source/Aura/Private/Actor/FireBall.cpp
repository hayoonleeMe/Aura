// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/FireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"
#include "Component/PooledActorComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interface/ObjectPoolInterface.h"

AFireBall::AFireBall()
{
	ProjectileMovementComponent->InitialSpeed = 650.f;
	ProjectileMovementComponent->MaxSpeed = 650.f;
	
	FireEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Fire Effect Component"));
	FireEffectComponent->SetupAttachment(GetRootComponent());
	ImpactCueTag = AuraGameplayTags::GameplayCue_FireBallImpact;

	PooledActorComponent = CreateDefaultSubobject<UPooledActorComponent>(TEXT("Pooled Actor Component"));
    PooledActorComponent->bAutoActivate = true;
}

void AFireBall::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	PooledActorComponent->SetInUseDelegate.BindUObject(this, &ThisClass::SetInUse);
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
	if (!IsValidOverlap(OtherActor))
	{
		return;
	}

	// Can apply damage only in server
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;	
			UAuraBlueprintLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}

	if (ImpactCueTag.IsValid())
	{
		UAuraBlueprintLibrary::ExecuteGameplayCue(GetOwner(), ImpactCueTag, GetActorLocation());
	}

	SpawnEmberBolts();

	PooledActorComponent->SetInUse(false);
}

bool AFireBall::IsValidOverlap(const AActor* TargetActor) const
{
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent ? DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	return IsValid(SourceAvatarActor) && IsValid(TargetActor) && SourceAvatarActor != TargetActor && UAuraBlueprintLibrary::IsNotFriend(SourceAvatarActor, TargetActor);
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
		if (IObjectPoolInterface* ObjectPoolInterface = Cast<IObjectPoolInterface>(GetOwner()))
		{
			if (AAuraProjectile* AuraProjectile = ObjectPoolInterface->SpawnFromPool<AAuraProjectile>(EPooledActorType::EmberBolt, SpawnTransform))
			{
				// Projectile로 데미지를 입히기 위해 설정
				AuraProjectile->DamageEffectParams = DamageEffectParams;
				AuraProjectile->DamageEffectParams.BaseDamage = EmberBoltDamage;
			}
		}	
	}
}

void AFireBall::SetInUse(bool bInUse) const
{
	if (bInUse)
	{
		ProjectileMovementComponent->Velocity = GetActorForwardVector() * ProjectileMovementComponent->InitialSpeed;
		ProjectileMovementComponent->Activate();
		SphereComponent->Activate();
		if (LoopingSoundComponent->GetSound())
		{
			LoopingSoundComponent->Activate();
		}
		FireEffectComponent->Activate();
	}
	else
	{
		ProjectileMovementComponent->Deactivate();
		SphereComponent->Deactivate();
		LoopingSoundComponent->Deactivate();
		FireEffectComponent->Deactivate();
	}
}
