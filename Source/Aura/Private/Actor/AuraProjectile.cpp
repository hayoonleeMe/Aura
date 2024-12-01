// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicatingMovement(true);
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SetRootComponent(SphereComponent);
	SphereComponent->SetCollisionObjectType(ECC_Projectile);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FCollisionResponseContainer Container;
	Container.SetAllChannels(ECR_Ignore);
	Container.SetResponse(ECC_Pawn, ECR_Overlap);
	Container.SetResponse(ECC_WorldStatic, ECR_Overlap);
	Container.SetResponse(ECC_WorldDynamic, ECR_Overlap);
	SphereComponent->SetCollisionResponseToChannels(Container);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovementComponent->InitialSpeed = 550.f;
	ProjectileMovementComponent->MaxSpeed = 550.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	
	LifeSpan = 15.f;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Actor의 수명 설정
	SetLifeSpan(LifeSpan);
	
	if (LoopingSound)
	{
		LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
	}
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
}

void AAuraProjectile::Destroyed()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	Super::Destroyed();
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
	
	ExecuteImpactCue();
	Destroy();
}

void AAuraProjectile::ExecuteImpactCue() const
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		FGameplayCueParameters CueParameters;
		CueParameters.Location = GetActorLocation();
		ASC->ExecuteGameplayCue(FAuraGameplayTags::Get().GameplayCue_FireBoltImpact, CueParameters);
	}
}

bool AAuraProjectile::IsValidOverlap(const AActor* TargetActor) const
{
	// Overlap occur server only
	if (!HasAuthority())
	{
		return false;
	}
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	return IsValid(SourceAvatarActor) && IsValid(TargetActor) && SourceAvatarActor != TargetActor && UAuraBlueprintLibrary::IsNotFriend(SourceAvatarActor, TargetActor); 
}
