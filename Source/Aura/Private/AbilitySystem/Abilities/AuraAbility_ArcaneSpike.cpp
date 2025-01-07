// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_ArcaneSpike.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_TargetDataUnderMouse.h"
#include "Aura/Aura.h"
#include "Types/DamageEffectParams.h"

UAuraAbility_ArcaneSpike::UAuraAbility_ArcaneSpike()
{
	MaxCastRange = 1400.f;
	CachedTargetLocation = FVector::ZeroVector;
}

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
	if (HitResult.GetActor())
	{
		CachedTargetLocation = UAuraBlueprintLibrary::GetActorFeetLocation(HitResult.GetActor()); 
	}
	if (CachedTargetLocation.IsZero())
	{
		CachedTargetLocation = HitResult.ImpactPoint;
	}

	const FTaggedCombatInfo TaggedCombatInfo = CombatInterface->GetTaggedCombatInfo(FAuraGameplayTags::Get().Abilities_Offensive_ArcaneSpike);
	check(TaggedCombatInfo.AnimMontage);

	// for Anim Montage Motion Warping
	CombatInterface->SetFacingTarget(CachedTargetLocation);

	PlayAttackMontage(TaggedCombatInfo.AnimMontage, true);
	WaitGameplayEvent(FAuraGameplayTags::Get().Event_Montage_ArcaneSpike);
}

void UAuraAbility_ArcaneSpike::OnEventReceived(FGameplayEventData Payload)
{
	SpawnArcaneShard();
	FinishAttack();
}

void UAuraAbility_ArcaneSpike::SpawnArcaneShard() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor) || !AvatarActor->HasAuthority())	// Only Spawn in Server
	{
		return;
	}

	// MaxCastRange를 적용한 TargetLocation 결정
	FVector TargetLocation = CachedTargetLocation;
	FVector StartLocation = AvatarActor->GetActorLocation();
	StartLocation.Z = TargetLocation.Z;
	if (FVector::DistSquared(StartLocation, TargetLocation) > MaxCastRange * MaxCastRange)
	{
		const FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();
		TargetLocation = StartLocation + Direction * MaxCastRange;
	}

	// 범위 결정
	const float ScaleRate = GetScaleRateByLevel(GetAbilityLevel());
	const float FinalEffectiveRadius = EffectiveRadius * ScaleRate;
	
	FGameplayCueParameters CueParameters;
	CueParameters.RawMagnitude = ScaleRate;

	// Spawn Arcane Shard
	const int32 NumArcaneShards = GetNumArcaneShardsByLevel(GetAbilityLevel());
	TArray<FVector> TargetLocations;
	if (NumArcaneShards == 1)
	{
		TargetLocations.Add(TargetLocation);
		CueParameters.Location = TargetLocation;
		UAuraBlueprintLibrary::ExecuteGameplayCueWithParams(AvatarActor, FAuraGameplayTags::Get().GameplayCue_ArcaneShard, CueParameters);
	}
	else
	{
		// TargetLocation 주위의 랜덤한 지점에 Arcane Shard를 소환한다.
		const float RandAngle = FMath::RandRange(0.f, 180.f);
		const float Angle = 360.f / NumArcaneShards;
		for (int32 Index = 0; Index < NumArcaneShards; ++Index)
		{
			FVector Offset = FVector::ForwardVector * FMath::RandRange(EffectiveRadius, FinalEffectiveRadius);
			const FVector FinalTargetLocation = TargetLocation + Offset.RotateAngleAxis(RandAngle + Angle * Index, FVector::UpVector);
			TargetLocations.Add(FinalTargetLocation);
			CueParameters.Location = FinalTargetLocation;
			UAuraBlueprintLibrary::ExecuteGameplayCueWithParams(AvatarActor, FAuraGameplayTags::Get().GameplayCue_ArcaneShard, CueParameters);
		}
	}

	// Arcane Shard 들에 의해 Damage를 입을 Enemies 결정
	TArray<AActor*> EnemiesToApplyDamage;
	for (const FVector& Location : TargetLocations)
	{
		TArray<AActor*> Enemies;
		UAuraBlueprintLibrary::GetEnemiesOverlappedByChannel(GetWorld(), Enemies, Location, FQuat::Identity, ECC_Target, FCollisionShape::MakeSphere(FinalEffectiveRadius));
		for (AActor* Enemy : Enemies)
		{
			EnemiesToApplyDamage.AddUnique(Enemy);
		}
	}

	// Apply Damage Effect
	FDamageEffectParams DamageEffectParams;
	for (AActor* Enemy : EnemiesToApplyDamage)
	{
		MakeDamageEffectParams(DamageEffectParams, Enemy);
		UAuraBlueprintLibrary::ApplyDamageEffect(DamageEffectParams);
	}
}

float UAuraAbility_ArcaneSpike::GetScaleRateByLevel(float Level) const
{
	return ScaleRateCurve.GetValueAtLevel(Level);
}

int32 UAuraAbility_ArcaneSpike::GetNumArcaneShardsByLevel(float Level) const
{
	return NumArcaneShardsCurve.GetValueAtLevel(Level);
}
