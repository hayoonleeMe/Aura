// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraBlueprintLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicatingMovement(true);
	bAlwaysRelevant = true;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SetRootComponent(SphereComponent);
	SphereComponent->SetCollisionObjectType(ECC_Projectile);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FCollisionResponseContainer Container;
	Container.SetAllChannels(ECR_Ignore);
	Container.SetResponse(ECC_Pawn, ECR_Overlap);
	Container.SetResponse(ECC_Enemy, ECR_Overlap);
	Container.SetResponse(ECC_WorldStatic, ECR_Overlap);
	Container.SetResponse(ECC_WorldDynamic, ECR_Overlap);
	SphereComponent->SetCollisionResponseToChannels(Container);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovementComponent->InitialSpeed = 550.f;
	ProjectileMovementComponent->MaxSpeed = 550.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	
	LoopingSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Looping Sound Component"));
	LoopingSoundComponent->SetupAttachment(GetRootComponent());
	LoopingSoundComponent->bAutoActivate = false;
}

void AAuraProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (LoopingSoundComponent->GetSound())
	{
		LoopingSoundComponent->Activate();
	}
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor))
	{
		return;
	}

	// Can apply damage
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		DamageEffectParams.TargetAbilitySystemComponent = TargetASC;	
		UAuraBlueprintLibrary::ApplyDamageEffect(DamageEffectParams);
	}
	
	if (ImpactCueTag.IsValid())
	{
		UAuraBlueprintLibrary::ExecuteGameplayCue(DamageEffectParams.SourceAbilitySystemComponent, ImpactCueTag, GetActorLocation());
	}

	Destroy();
}

bool AAuraProjectile::IsValidOverlap(const AActor* TargetActor) const
{
	// Overlap occur server only
	if (!HasAuthority())
	{
		return false;
	}
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent ? DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	return IsValid(SourceAvatarActor) && IsValid(TargetActor) && SourceAvatarActor != TargetActor && UAuraBlueprintLibrary::IsNotFriend(SourceAvatarActor, TargetActor); 
}
