// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/FireBolt.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"

AFireBolt::AFireBolt()
{
	FireEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Fire Effect Component"));
	FireEffectComponent->SetupAttachment(GetRootComponent());
	ImpactCueTag = AuraGameplayTags::GameplayCue_FireBoltImpact;

	PooledActorType = EPooledActorType::FireBolt;
}

void AFireBolt::SetInUse(bool bInUse)
{
	Super::SetInUse(bInUse);

	if (bInUse)
	{
		FireEffectComponent->Activate();
	}
	else
	{
		FireEffectComponent->Deactivate();
	}
}
