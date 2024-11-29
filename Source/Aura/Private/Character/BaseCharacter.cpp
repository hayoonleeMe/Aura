// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"

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
