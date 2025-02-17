// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/FireBolt_Pooled.h"

#include "NiagaraComponent.h"
#include "Component/PooledActorComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

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
