// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_EnemyMelee.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Types/AuraAbilityTypes.h"

UAuraAbility_EnemyMelee::UAuraAbility_EnemyMelee()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bCanAttackMultiTarget = true;
}

void UAuraAbility_EnemyMelee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Activated only in server by AI (BTTask)
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	const ICombatInterface* CombatInterface = Cast<ICombatInterface>(AvatarActor);
	if (!IsValid(AvatarActor) || !CombatInterface)
	{
		return;
	}

	// Get TaggedCombatInfo and Caching
	const FTaggedCombatInfo TaggedCombatInfo = ICombatInterface::Execute_GetTaggedCombatInfo(AvatarActor, FAuraGameplayTags::Get().Abilities_EnemyAttack);
	check(TaggedCombatInfo.AnimMontage);
	CachedCombatSocketName = TaggedCombatInfo.CombatSocketName;

	// for Anim Montage Motion Warping
	const TWeakObjectPtr<AActor> CombatTargetWeakPtr = CombatInterface->GetCombatTarget();
	if (CombatTargetWeakPtr.IsValid())
	{
		const FVector CombatTargetLocation = CombatTargetWeakPtr.Get()->GetActorLocation(); 
		ICombatInterface::Execute_SetFacingTarget(AvatarActor, CombatTargetLocation);
	}

	// Play attack montage
	if (UAbilityTask_PlayMontageAndWait* AbilityTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), TaggedCombatInfo.AnimMontage, 1.f, NAME_None, false))
	{
		AbilityTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		AbilityTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		AbilityTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		AbilityTask->ReadyForActivation();
	}

	// Wait gameplay event from attack montage
	if (UAbilityTask_WaitGameplayEvent* AbilityTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FAuraGameplayTags::Get().Event_Montage_EnemyAttack))
	{
		AbilityTask->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
		AbilityTask->ReadyForActivation();
	}
}

void UAuraAbility_EnemyMelee::OnEventReceived(FGameplayEventData Payload)
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor) || !AvatarActor->Implements<UCombatInterface>())
	{
		return;
	}

	float AttackRangeRadius = 0.f, AttackRangeHalfHeight = 0.f;
	ICombatInterface::Execute_GetAttackCheckRange(AvatarActor, AttackRangeRadius, AttackRangeHalfHeight);
	const FTransform CombatSocketTransform = ICombatInterface::Execute_GetCombatSocketTransform(AvatarActor, CachedCombatSocketName);

	FCollisionShape CollisionShape;
	CollisionShape.SetCapsule(AttackRangeRadius, AttackRangeHalfHeight);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	//DrawDebugCapsule(GetWorld(), CombatSocketTransform.GetLocation(), AttackRangeHalfHeight, AttackRangeRadius, CombatSocketTransform.GetRotation(), FColor::Green, false, 3.f);
	
	TArray<FOverlapResult> OverlapResults;
	if (!GetWorld()->OverlapMultiByChannel(OverlapResults, CombatSocketTransform.GetLocation(), CombatSocketTransform.GetRotation(), ECC_Visibility, CollisionShape, QueryParams))
	{
		return;
	}

	// Apply Damage
	FDamageEffectParams DamageEffectParams;
	MakeDamageEffectParams(DamageEffectParams, nullptr);	// Need to set TargetAbilitySystemComponent
	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		AActor* TargetActor = OverlapResult.GetActor();
		if (OverlapResult.bBlockingHit && IsValid(TargetActor) && UAuraBlueprintLibrary::IsNotFriend(AvatarActor, TargetActor))
		{
			DamageEffectParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
			if (DamageEffectParams.TargetAbilitySystemComponent)
			{
				UAuraBlueprintLibrary::ApplyDamageEffect(DamageEffectParams);

				// Execute EnemyMeleeImpact GameplayCue
				UAuraBlueprintLibrary::ExecuteGameplayCue(TargetActor, FAuraGameplayTags::Get().GameplayCue_EnemyMeleeImpact, TargetActor->GetActorLocation());
			}
		}

		// 한번에 여러 대상을 공격할 수 없으면 하나의 대상만 공격하고 빠져나간다.
		if (!bCanAttackMultiTarget)
		{
			break;
		}
	}
}
