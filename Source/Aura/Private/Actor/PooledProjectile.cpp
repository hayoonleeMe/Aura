// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/PooledProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraBlueprintLibrary.h"
#include "Component/PooledActorComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

APooledProjectile::APooledProjectile()
{
	PooledActorComponent = CreateDefaultSubobject<UPooledActorComponent>(TEXT("Pooled Actor Component"));
	PooledActorComponent->bAutoActivate = true;
	PooledActorComponent->SetInUseDelegate.BindUObject(this, &ThisClass::OnSetInUse);
}

void APooledProjectile::SetInUse(bool bInUse) const
{
	PooledActorComponent->SetInUse(bInUse);
}

void APooledProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
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
		UAuraBlueprintLibrary::ExecuteGameplayCue(DamageEffectParams.SourceAbilitySystemComponent, ImpactCueTag, GetActorLocation());
	}

	PooledActorComponent->SetInUse(false);
}

bool APooledProjectile::IsValidOverlap(const AActor* TargetActor) const
{
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent ? DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	return IsValid(SourceAvatarActor) && IsValid(TargetActor) && SourceAvatarActor != TargetActor && UAuraBlueprintLibrary::IsNotFriend(SourceAvatarActor, TargetActor);
}

void APooledProjectile::OnSetInUse(bool bInUse)
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
	}
	else
	{
		ProjectileMovementComponent->Deactivate();
		SphereComponent->Deactivate();
		LoopingSoundComponent->Deactivate();
	}
}
