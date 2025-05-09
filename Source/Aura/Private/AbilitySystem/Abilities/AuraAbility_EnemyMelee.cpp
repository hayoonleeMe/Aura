// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_EnemyMelee.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "Interface/CombatInterface.h"
#include "Types/DamageEffectParams.h"

UAuraAbility_EnemyMelee::UAuraAbility_EnemyMelee()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(AuraGameplayTags::Abilities_EnemyAttack);
	bCanAttackMultiTarget = true;
	DamageTypeTag = AuraGameplayTags::Damage_Type_Physical;
	bNeedCursorTargetHitResult = false;
}

void UAuraAbility_EnemyMelee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Activated only in server by AI (BTTask)
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)
	{
		return;
	}

	// Get TaggedCombatInfo and Caching
	const FTaggedCombatInfo TaggedCombatInfo = CombatInterface->GetTaggedCombatInfo(AuraGameplayTags::Abilities_EnemyAttack);
	check(TaggedCombatInfo.AnimMontage);
	CachedCombatSocketName = TaggedCombatInfo.CombatSocketName;

	// for Anim Montage Motion Warping
	const TWeakObjectPtr<AActor> CombatTargetWeakPtr = CombatInterface->GetCombatTarget();
	if (CombatTargetWeakPtr.IsValid())
	{
		const FVector CombatTargetLocation = CombatTargetWeakPtr.Get()->GetActorLocation(); 
		CombatInterface->SetFacingTarget(CombatTargetLocation);
	}

	PlayAttackMontage(TaggedCombatInfo.AnimMontage, false);
	WaitGameplayEvent(AuraGameplayTags::Event_Montage_EnemyAttack);
}

void UAuraAbility_EnemyMelee::OnEventReceived(FGameplayEventData Payload)
{
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)
	{
		return;
	}

	Super::OnEventReceived(Payload);

	float AttackRangeRadius = 0.f, AttackRangeHalfHeight = 0.f;
	CombatInterface->GetAttackCheckRange(AttackRangeRadius, AttackRangeHalfHeight);
	const FTransform CombatSocketTransform = CombatInterface->GetCombatSocketTransform(CachedCombatSocketName);

	FCollisionShape CollisionShape;
	CollisionShape.SetCapsule(AttackRangeRadius, AttackRangeHalfHeight);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());

	//DrawDebugCapsule(GetWorld(), CombatSocketTransform.GetLocation(), AttackRangeHalfHeight, AttackRangeRadius, CombatSocketTransform.GetRotation(), FColor::Green, false, 3.f);
	
	TArray<FOverlapResult> OverlapResults;
	if (GetWorld()->OverlapMultiByChannel(OverlapResults, CombatSocketTransform.GetLocation(), CombatSocketTransform.GetRotation(), ECC_Visibility, CollisionShape, QueryParams))
	{
		// Apply Damage
		FDamageEffectParams DamageEffectParams = MakeDamageEffectParams(nullptr);	// Need to set TargetAbilitySystemComponent
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			AActor* TargetActor = OverlapResult.GetActor();
			if (OverlapResult.bBlockingHit && IsValid(TargetActor) && UAuraBlueprintLibrary::IsNotFriend(GetAvatarActorFromActorInfo(), TargetActor))
			{
				DamageEffectParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
				if (DamageEffectParams.TargetAbilitySystemComponent)
				{
					if (UAuraBlueprintLibrary::ApplyDamageEffect(DamageEffectParams))
					{
						// 성공적으로 데미지를 입힐 때 Impact Cue 실행
						UAuraBlueprintLibrary::ExecuteGameplayCue(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor), AuraGameplayTags::GameplayCue_EnemyMeleeImpact, TargetActor->GetActorLocation());
					}
				}
			}

			// 한번에 여러 대상을 공격할 수 없으면 하나의 대상만 공격하고 빠져나간다.
			if (!bCanAttackMultiTarget)
			{
				break;
			}
		}
	}

	FinishAttack();
}
