// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraDamageAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interface/PlayerInterface.h"
#include "Types/DamageEffectParams.h"
#include "GameFramework/PlayerController.h"

UAuraDamageAbility::UAuraDamageAbility()
{
	// 어빌리티를 실행하면 현재 실행 중인 Confirmation을 기다리는 어빌리티를 취소한다.
	CancelAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_NeedConfirm);
	
	bFinishMontage = false;
	bFinishAttack = false;
	bShouldClearTargetActor = false;
	bNeedCursorTargetHitResult = true;
}

FDamageEffectParams UAuraDamageAbility::MakeDamageEffectParams(AActor* TargetActor) const
{
	FDamageEffectParams RetParams;
	RetParams.WorldContextObject = GetAvatarActorFromActorInfo();
	RetParams.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	RetParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	RetParams.DamageEffectClass = DamageEffectClass;
	RetParams.BaseDamage = GetDamageByLevel(GetAbilityLevel());
	RetParams.DamageTypeTag = DamageTypeTag;
	RetParams.AbilityLevel = GetAbilityLevel();
	RetParams.DebuffTag = DebuffTag;
	RetParams.DebuffChance = GetDebuffChanceByLevel(GetAbilityLevel());
	return RetParams;
}

float UAuraDamageAbility::GetDamageByLevel(int32 Level) const
{
	return DamageCurve.GetValueAtLevel(Level);
}

void UAuraDamageAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	bShouldClearTargetActor = true;
}

void UAuraDamageAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	bFinishMontage = false;
	bFinishAttack = false;
	bShouldClearTargetActor = false;
}

bool UAuraDamageAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (IsLocallyControlled() && bNeedCursorTargetHitResult)
	{
		if (const IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(ActorInfo ? ActorInfo->PlayerController : nullptr))
		{
			return PlayerInterface->GetTargetHitResult().IsValidBlockingHit();
		}
	}

	return true;
}

void UAuraDamageAbility::PlayAttackMontage(UAnimMontage* MontageToPlay, bool bEndOnBlendOut)
{
	if (UAbilityTask_PlayMontageAndWait* AbilityTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), MontageToPlay, 1.f, NAME_None, false))
	{
		if (bEndOnBlendOut)
		{
			AbilityTask->OnBlendOut.AddDynamic(this, &ThisClass::FinishMontage);
		}
		AbilityTask->OnCompleted.AddDynamic(this, &ThisClass::FinishMontage);
		AbilityTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		AbilityTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		AbilityTask->ReadyForActivation();
	}
}

void UAuraDamageAbility::WaitGameplayEvent(const FGameplayTag& EventTag)
{
	// Wait gameplay event from attack montage
	if (UAbilityTask_WaitGameplayEvent* AbilityTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag, nullptr, true))
	{
		AbilityTask->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
		AbilityTask->ReadyForActivation();
	}
}

void UAuraDamageAbility::FinishMontage()
{
	bFinishMontage = true;
	TryEndAbility();
}

void UAuraDamageAbility::OnEventReceived(FGameplayEventData Payload)
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		K2_CommitAbility();
	}
}

void UAuraDamageAbility::FinishAttack()
{
	bFinishAttack = true;
	TryEndAbility();
}

void UAuraDamageAbility::TryEndAbility()
{
	if (bFinishMontage && bFinishAttack)
	{
		K2_EndAbility();
	}
}

void UAuraDamageAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Ability 실행 중에 InputReleased()가 호출된 상태, 다음 실행부터 TargetActor를 다시 결정하도록 초기화해준다. 
	if (bShouldClearTargetActor)
	{
		if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
		{
			AuraASC->CursorTargetWeakPtr = nullptr;
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

float UAuraDamageAbility::GetDebuffChanceByLevel(float Level) const
{
	return DebuffChanceCurve.GetValueAtLevel(Level);
}
