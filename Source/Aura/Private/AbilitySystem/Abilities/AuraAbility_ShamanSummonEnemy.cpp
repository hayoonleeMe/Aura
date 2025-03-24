// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_ShamanSummonEnemy.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/AuraEnemy.h"
#include "Game/StageGameMode.h"
#include "Interface/CombatInterface.h"

UAuraAbility_ShamanSummonEnemy::UAuraAbility_ShamanSummonEnemy()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(AuraGameplayTags::Abilities_ShamanSummonEnemy);
	BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_HitReact);

	SpawnConeHalfAngleDegrees = 60.f;
	SpawnMinDistance = 70.f;
	SpawnMaxDistance = 200.f;
	SpawnWaitTime = 2.f;
	RandomDeviation = 0.5f;
	SpawnDelayTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::AsyncSpawnEnemies);
}

void UAuraAbility_ShamanSummonEnemy::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                     const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(SummonMontage);
	check(EnemyClasses.Num());

	// Activated only in server by AI (BTTask)
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)
	{
		return;
	}

	// for Anim Montage Motion Warping
	const TWeakObjectPtr<AActor> CombatTargetWeakPtr = CombatInterface->GetCombatTarget();
	if (CombatTargetWeakPtr.IsValid())
	{
		const FVector CombatTargetLocation = CombatTargetWeakPtr.Get()->GetActorLocation(); 
		CombatInterface->SetFacingTarget(CombatTargetLocation);
	}

	if (UAbilityTask_PlayMontageAndWait* AbilityTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), SummonMontage))
	{
		// 모든 Enemy를 소환하면 Ability 종료와 함께 Stop
		AbilityTask->ReadyForActivation();
	}

	// Wait gameplay event from summon montage
	if (UAbilityTask_WaitGameplayEvent* AbilityTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, AuraGameplayTags::Event_Montage_ShamanSummonEnemy, nullptr, true))
	{
		AbilityTask->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
		AbilityTask->ReadyForActivation();
	}
}

void UAuraAbility_ShamanSummonEnemy::OnEventReceived(FGameplayEventData Payload)
{
	// Spawn Enemy
	StageGameMode = GetWorld()->GetAuthGameMode<AStageGameMode>();	
	MaxSummonCount = GetNumEnemies();
	AsyncSpawnEnemies();
}

int32 UAuraAbility_ShamanSummonEnemy::GetNumEnemies() const
{
	return NumEnemiesCurve.GetValueAtLevel(GetAbilityLevel());
}

const TSubclassOf<AAuraEnemy>& UAuraAbility_ShamanSummonEnemy::GetRandomEnemyClass() const
{
	return EnemyClasses[FMath::RandRange(0, EnemyClasses.Num() - 1)];
}

void UAuraAbility_ShamanSummonEnemy::AsyncSpawnEnemies()
{
	if (NumSummonedEnemies < MaxSummonCount)
	{
		if (const AActor* AvatarActor = GetAvatarActorFromActorInfo())
		{
			++NumSummonedEnemies;
			
			// Find RandomPoint
			const FVector RandomDir = FMath::VRandCone(AvatarActor->GetActorForwardVector(), FMath::DegreesToRadians(SpawnConeHalfAngleDegrees));
			const FVector RandomPoint = AvatarActor->GetActorLocation() + RandomDir * FMath::RandRange(SpawnMinDistance, SpawnMaxDistance);

			if (StageGameMode)
			{
				StageGameMode->RequestSpawnEnemy(GetRandomEnemyClass(), RandomPoint, true);
			}
		}

		if (NumSummonedEnemies < MaxSummonCount)
		{
			// RandomDelay가 지난 뒤 다시 소환
			GetWorld()->GetTimerManager().SetTimer(SpawnDelayTimerHandle, SpawnDelayTimerDelegate/*AsyncSpawnEnemies*/, GetRandomDelay(), false);	
		}
		else
		{
			K2_EndAbility();
		}
	}
}
