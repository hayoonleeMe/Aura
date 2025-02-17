// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/FireBolt_Pooled.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Component/PooledActorComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"

AFireBolt_Pooled::AFireBolt_Pooled()
{
	PooledActorComponent = CreateDefaultSubobject<UPooledActorComponent>(TEXT("Pooled Actor Component"));
	PooledActorComponent->bAutoActivate = true;
}

void AFireBolt_Pooled::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PooledActorComponent->SetInUseDelegate.BindUObject(this, &ThisClass::SetInUse);
}

void AFireBolt_Pooled::SetInUse(bool bInUse) const
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

void AFireBolt_Pooled::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
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

	PooledActorComponent->SetInUse(false);
}

bool AFireBolt_Pooled::IsValidOverlap(const AActor* TargetActor) const
{
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent ? DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	return IsValid(SourceAvatarActor) && IsValid(TargetActor) && SourceAvatarActor != TargetActor && UAuraBlueprintLibrary::IsNotFriend(SourceAvatarActor, TargetActor); 
}
