// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/FireBolt.h"

#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"

AFireBolt::AFireBolt()
{
	FireEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Fire Effect Component"));
	FireEffectComponent->SetupAttachment(GetRootComponent());

	ImpactCueTag = AuraGameplayTags::GameplayCue_FireBoltImpact;
}



