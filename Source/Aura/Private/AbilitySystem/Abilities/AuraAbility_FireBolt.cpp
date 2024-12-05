// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_FireBolt.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_TargetDataUnderMouse.h"

void UAuraAbility_FireBolt::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (UAbilityTask_TargetDataUnderMouse* AbilityTask = UAbilityTask_TargetDataUnderMouse::CreateTask(this))
	{
		AbilityTask->TargetDataUnderMouseSetDelegate.BindUObject(this, &ThisClass::OnTargetDataUnderMouseSet);
		AbilityTask->ReadyForActivation();
	}
}

void UAuraAbility_FireBolt::OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor) || !AvatarActor->Implements<UCombatInterface>())
	{
		return;
	}

	// Caching
	const FHitResult& HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
	CachedTargetLocation = HitResult.ImpactPoint;
	
	const FTaggedCombatInfo TaggedCombatInfo = ICombatInterface::Execute_GetTaggedCombatInfo(AvatarActor, FAuraGameplayTags::Get().Abilities_FireBolt);
	check(TaggedCombatInfo.AnimMontage);
	CachedCombatSocketName = TaggedCombatInfo.CombatSocketName;

	// for Anim Montage Motion Warping
	ICombatInterface::Execute_SetFacingTarget(GetAvatarActorFromActorInfo(), CachedTargetLocation);

	if (UAbilityTask_PlayMontageAndWait* AbilityTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), TaggedCombatInfo.AnimMontage, 1.f, NAME_None, false))
	{
		AbilityTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		AbilityTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		AbilityTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		AbilityTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		AbilityTask->ReadyForActivation();
	}
	if (UAbilityTask_WaitGameplayEvent* AbilityTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FAuraGameplayTags::Get().Event_Montage_FireBolt))
	{
		AbilityTask->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
		AbilityTask->ReadyForActivation();
	}
}

void UAuraAbility_FireBolt::OnEventReceived(FGameplayEventData Payload)
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor) || !AvatarActor->Implements<UCombatInterface>())
	{
		return;
	}
	
	const FVector CombatSocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor, CachedCombatSocketName);
	SpawnProjectile(CachedTargetLocation, CombatSocketLocation);
}
