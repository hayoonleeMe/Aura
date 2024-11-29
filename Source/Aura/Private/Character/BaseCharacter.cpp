// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"

#include "MotionWarpingComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	/* Weapon Mesh */
	WeaponSocketName = TEXT("WeaponHandSocket");
	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh Component"));
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMeshComponent->SetupAttachment(GetMesh(), WeaponSocketName);
	CombatSocketName = TEXT("CombatSocket");

	/* Motion Warping */
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping Component"));
	WarpTargetName = TEXT("FacingTarget");
}

FVector ABaseCharacter::GetCombatSocketLocation_Implementation() const
{
	if (WeaponMeshComponent)
	{
		return WeaponMeshComponent->GetSocketLocation(CombatSocketName);
	}
	return FVector::ZeroVector;
}

void ABaseCharacter::SetFacingTarget_Implementation(const FVector& TargetLocation)
{
	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(WarpTargetName, TargetLocation);
	}
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::ApplyEffectSpecToSelf(const TSubclassOf<UGameplayEffect>& EffectClass, float InLevel) const
{
	if (EffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(EffectClass, InLevel, ContextHandle);
		GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
}

void ABaseCharacter::AddStartupAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilityClasses) const
{
	if (HasAuthority())
	{
		UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
		AuraASC->AddAbilities(AbilityClasses);
	}
}
