// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_EnemyRange.h"

#include "AuraGameplayTags.h"
#include "Interaction/CombatInterface.h"

UAuraAbility_EnemyRange::UAuraAbility_EnemyRange()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UAuraAbility_EnemyRange::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		return;
	}
	
	// Activated only in server by AI (BTTask)
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	const ICombatInterface* CombatInterface = Cast<ICombatInterface>(AvatarActor);
	if (!IsValid(AvatarActor) || !CombatInterface)
	{
		return;
	}

	// Caching
	const FTaggedCombatInfo TaggedCombatInfo = ICombatInterface::Execute_GetTaggedCombatInfo(AvatarActor, FAuraGameplayTags::Get().Abilities_EnemyAttack);
	check(TaggedCombatInfo.AnimMontage);
	CachedCombatSocketName = TaggedCombatInfo.CombatSocketName;

	// for Anim Montage Motion Warping
	// for Spawn Projectile
	const TWeakObjectPtr<AActor> CombatTargetWeakPtr = CombatInterface->GetCombatTarget();
	if (CombatTargetWeakPtr.IsValid())
	{
		CachedTargetLocation = CombatTargetWeakPtr.Get()->GetActorLocation(); 
		ICombatInterface::Execute_SetFacingTarget(AvatarActor, CachedTargetLocation);
	}

	PlayAttackMontage(TaggedCombatInfo.AnimMontage, false);
	WaitGameplayEvent(FAuraGameplayTags::Get().Event_Montage_EnemyAttack);
}

void UAuraAbility_EnemyRange::OnEventReceived(FGameplayEventData Payload)
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor) || !AvatarActor->Implements<UCombatInterface>())
	{
		return;
	}

	const FVector& CombatSocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor, CachedCombatSocketName);
	SpawnProjectile(CachedTargetLocation, CombatSocketLocation);

	FinishAttack();
}
