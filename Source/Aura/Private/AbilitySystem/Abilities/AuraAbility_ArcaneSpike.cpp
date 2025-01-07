// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_ArcaneSpike.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_TargetDataUnderMouse.h"
#include "Interaction/CombatInterface.h"

void UAuraAbility_ArcaneSpike::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		return;
	}
	
	if (UAbilityTask_TargetDataUnderMouse* AbilityTask = UAbilityTask_TargetDataUnderMouse::CreateTask(this))
	{
		AbilityTask->TargetDataUnderMouseSetDelegate.BindUObject(this, &ThisClass::OnTargetDataUnderMouseSet);
		AbilityTask->ReadyForActivation();
	}
}

void UAuraAbility_ArcaneSpike::OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)
	{
		return;
	}

	const FHitResult& HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
	CachedImpactPoint = HitResult.ImpactPoint;

	const FTaggedCombatInfo TaggedCombatInfo = CombatInterface->GetTaggedCombatInfo(FAuraGameplayTags::Get().Abilities_Offensive_ArcaneSpike);
	check(TaggedCombatInfo.AnimMontage);

	// for Anim Montage Motion Warping
	CombatInterface->SetFacingTarget(CachedImpactPoint);

	PlayAttackMontage(TaggedCombatInfo.AnimMontage, true);
	WaitGameplayEvent(FAuraGameplayTags::Get().Event_Montage_ArcaneSpike);
}

void UAuraAbility_ArcaneSpike::OnEventReceived(FGameplayEventData Payload)
{
	SpawnArcaneShard();
	FinishAttack();
}

void UAuraAbility_ArcaneSpike::SpawnArcaneShard()
{
	const UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AuraASC || !IsValid(AvatarActor) || !AvatarActor->HasAuthority())	// Only Spawn in Server
	{
		return;
	}

	// TODO : 최대 시전 사거리로 Clamping한 TargetLocation 결정
	const FVector TargetLocation = CachedImpactPoint;

	// Spawn Arcane Shard
	for (int32 Index = 0; Index < 1 /* TODO : Parameterize */; ++Index)
	{
		UAuraBlueprintLibrary::ExecuteGameplayCue(AvatarActor, FAuraGameplayTags::Get().GameplayCue_ArcaneShard, TargetLocation);
	}
}
