// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_EnemyRange.h"

#include "AuraGameplayTags.h"
#include "Interface/CombatInterface.h"

UAuraAbility_EnemyRange::UAuraAbility_EnemyRange()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(AuraGameplayTags::Abilities_EnemyAttack);
	DamageTypeTag = AuraGameplayTags::Damage_Type_Physical;
}

void UAuraAbility_EnemyRange::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Activated only in server by AI (BTTask)
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)
	{
		return;
	}

	// Caching
	const FTaggedCombatInfo TaggedCombatInfo = CombatInterface->GetTaggedCombatInfo(AuraGameplayTags::Abilities_EnemyAttack);
	check(TaggedCombatInfo.AnimMontage);
	CachedCombatSocketName = TaggedCombatInfo.CombatSocketName;

	// for Anim Montage Motion Warping
	// for Spawn Projectile
	const TWeakObjectPtr<AActor> CombatTargetWeakPtr = CombatInterface->GetCombatTarget();
	if (CombatTargetWeakPtr.IsValid())
	{
		CachedTargetLocation = CombatTargetWeakPtr.Get()->GetActorLocation(); 
		CombatInterface->SetFacingTarget(CachedTargetLocation);
	}

	PlayAttackMontage(TaggedCombatInfo.AnimMontage, false);
	WaitGameplayEvent(AuraGameplayTags::Event_Montage_EnemyAttack);
}

void UAuraAbility_EnemyRange::OnEventReceived(FGameplayEventData Payload)
{
	const ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)
	{
		return;
	}

	Super::OnEventReceived(Payload);

	const FVector& CombatSocketLocation = CombatInterface->GetCombatSocketLocation(CachedCombatSocketName);
	SpawnProjectile(CachedTargetLocation, CombatSocketLocation);

	FinishAttack();
}
