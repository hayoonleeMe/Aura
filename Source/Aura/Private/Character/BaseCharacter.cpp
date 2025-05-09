// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"

#include "MotionWarpingComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EQS_Overlap, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_ClickToMove, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_ClickToMove, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Enemy, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	/* Weapon Mesh */
	WeaponSocketName = TEXT("WeaponHandSocket");
	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh Component"));
	WeaponMeshComponent->SetCollisionObjectType(ECC_Pawn);	// same with mesh
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMeshComponent->SetupAttachment(GetMesh(), WeaponSocketName);

	/* Motion Warping */
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping Component"));
	WarpTargetName = TEXT("FacingTarget");

	/* Movement */
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, bDead);
}

void ABaseCharacter::SetFacingTarget(const FVector& TargetLocation)
{
	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(WarpTargetName, TargetLocation);
	}
}

FTaggedCombatInfo ABaseCharacter::GetTaggedCombatInfo(const FGameplayTag& InTag) const
{
	// InTag를 가지는 랜덤한 FTaggedCombatInfo를 반환 
	const TArray Filtered = TaggedCombatInfos.FilterByPredicate([InTag](const FTaggedCombatInfo& TaggedCombatInfo)
	{
		return TaggedCombatInfo.Tag == InTag;
	});
	check(!Filtered.IsEmpty());

	// 랜덤한 원소 반환
	const int32 RandIndex = FMath::RandRange(0, Filtered.Num() - 1);
	return Filtered[RandIndex];
}

FVector ABaseCharacter::GetCombatSocketLocation(const FName& CombatSocketName) const
{
	return GetCombatSocketTransform(CombatSocketName).GetLocation();
}

FTransform ABaseCharacter::GetCombatSocketTransform(const FName& CombatSocketName) const
{
	if (WeaponMeshComponent && WeaponMeshComponent->DoesSocketExist(CombatSocketName))
	{
		return WeaponMeshComponent->GetSocketTransform(CombatSocketName);
	}
	if (GetMesh() && GetMesh()->DoesSocketExist(CombatSocketName))
	{
		return GetMesh()->GetSocketTransform(CombatSocketName);
	}
	return FTransform::Identity;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::AddStartupAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilityClasses, int32 InLevel) const
{
	if (HasAuthority())
	{
		UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
		AuraASC->AddAbilities(AbilityClasses, InLevel);
	}
}

void ABaseCharacter::Die()
{
	/* Called on server */
	
	// 중복 호출 방지
	if (!bDead)
	{
		bDead = true;
		HandleDeathLocally();
	}
}

void ABaseCharacter::OnRep_Dead()
{
	if (bDead)
	{
		// Rep Notify로 Multicast RPC를 대신함
		HandleDeathLocally();
	}
}
