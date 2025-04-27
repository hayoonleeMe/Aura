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
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(AuraGameplayTags::Abilities_Offensive_ArcaneSpike);
	CancelAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_ClickToMove);
	CancelAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_TryInteract);
	BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_ClickToMove);
	BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_Offensive);
	DamageTypeTag = AuraGameplayTags::Damage_Type_Arcane;
	DebuffTag = AuraGameplayTags::Debuff_Enfeeble;
	MaxCastRange = 1400.f;
	CachedTargetLocation = FVector::ZeroVector;
}

FText UAuraAbility_ArcaneSpike::GetDescription(int32 Level) const
{
	const float ScaledDamage = GetDamageByLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);

	const int32 NumArcaneShards = GetNumArcaneShardsByLevel(Level);

	const int32 DebuffChance = GetDebuffChanceByLevel(Level);
	
	FString PartStr = TEXT("<Default>Summon a shard of arcane energy, ");
	if (Level != 1)
	{
		PartStr = FString::Printf(TEXT("<Default>Summon </><Damage>%d </><Default>shards of arcane energy, "), NumArcaneShards);
	}
	
	const FString RetStr = FString::Printf(TEXT(
		// Title
		"<Title>ARCANE SPIKE</>\n\n"

		// Level
		"<Default>Level: </><Level>%d</>\n"
		// ManaCost
		"<Default>ManaCost: </><ManaCost>%.1f</>\n"
		// Cooldown
		"<Default>Cooldown: </><Cooldown>%.1f</>\n\n"

		// NumArcaneShards
		"%s"
		"causing radial arcane damage of </>"

		// Damage
		"<Damage>%.1f</><Default>. "

		// Debuff
		"This spell has a </><Percent>%d%% </><Default>chance to enfeeble the target on hit. "
		"Enfeeble reduces the target's damage by </><Percent>20%% </><Default>for </><Time>10 </><Default>seconds. Can't stack.</>"),

		// Values
		Level,
		ManaCost,
		Cooldown,
		*PartStr,
		ScaledDamage,
		DebuffChance
	);
	
	return FText::FromString(RetStr);
}

void UAuraAbility_ArcaneSpike::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAuraDamageAbility::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
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

	const FTaggedCombatInfo TaggedCombatInfo = CombatInterface->GetTaggedCombatInfo(AuraGameplayTags::Abilities_Offensive_ArcaneSpike);
	check(TaggedCombatInfo.AnimMontage);

	// for Anim Montage Motion Warping
	CombatInterface->SetFacingTarget(CachedTargetLocation);

	PlayAttackMontage(TaggedCombatInfo.AnimMontage, true);
	WaitGameplayEvent(AuraGameplayTags::Event_Montage_ArcaneSpike);
}

void UAuraAbility_ArcaneSpike::OnEventReceived(FGameplayEventData Payload)
{
	Super::OnEventReceived(Payload);
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

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());
	
	FGameplayCueParameters CueParameters;
	CueParameters.RawMagnitude = ScaleRate;

	// Spawn Arcane Shard
	const int32 NumArcaneShards = GetNumArcaneShardsByLevel(GetAbilityLevel());
	TArray<FVector> TargetLocations;
	if (NumArcaneShards == 1)
	{
		const FVector FinalTargetLocation = GetAdjustedTargetLocation(StartLocation, TargetLocation, QueryParams, true);
		TargetLocations.Add(FinalTargetLocation);
		CueParameters.Location = FinalTargetLocation;
		UAuraBlueprintLibrary::ExecuteGameplayCueWithParams(AvatarActor, AuraGameplayTags::GameplayCue_ArcaneShard, CueParameters);
	}
	else
	{
		TargetLocation = GetAdjustedTargetLocation(StartLocation, TargetLocation, QueryParams, true);
		
		// TargetLocation 주위의 랜덤한 지점에 Arcane Shard를 소환한다.
		const float RandAngle = FMath::RandRange(0.f, 180.f);
		const float Angle = 360.f / NumArcaneShards;
		for (int32 Index = 0; Index < NumArcaneShards; ++Index)
		{
			FVector Offset = FVector::ForwardVector * FMath::RandRange(EffectiveRadius, FinalEffectiveRadius);
			FVector FinalTargetLocation = TargetLocation + Offset.RotateAngleAxis(RandAngle + Angle * Index, FVector::UpVector);
			FinalTargetLocation = GetAdjustedTargetLocation(StartLocation, FinalTargetLocation, QueryParams, false);
			TargetLocations.Add(FinalTargetLocation);
			CueParameters.Location = FinalTargetLocation;
			UAuraBlueprintLibrary::ExecuteGameplayCueWithParams(AvatarActor, AuraGameplayTags::GameplayCue_ArcaneShard, CueParameters);
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
	for (AActor* Enemy : EnemiesToApplyDamage)
	{
		UAuraBlueprintLibrary::ApplyDamageEffect(MakeDamageEffectParams(Enemy));
	}
}

FVector UAuraAbility_ArcaneSpike::GetAdjustedTargetLocation(const FVector& StartLocation,  const FVector& TargetLocation, const FCollisionQueryParams& QueryParams, bool bDoLineTrace) const
{
	if (bDoLineTrace)
	{
		FHitResult LineHitResult;
		if (GetWorld()->LineTraceSingleByChannel(LineHitResult, StartLocation, TargetLocation, ECC_OnlyWall, QueryParams))
		{
			return LineHitResult.ImpactPoint + LineHitResult.ImpactNormal * EffectiveRadius;
		}
	}
	FHitResult SweepHitResult;
	if (GetWorld()->SweepSingleByChannel(SweepHitResult, TargetLocation + -TargetLocation.GetSafeNormal() * EffectiveRadius, TargetLocation, FQuat::Identity, ECC_OnlyWall, FCollisionShape::MakeSphere(EffectiveRadius), QueryParams))
	{
		return SweepHitResult.ImpactPoint + SweepHitResult.ImpactNormal * EffectiveRadius;
	}
	return TargetLocation;
}

float UAuraAbility_ArcaneSpike::GetScaleRateByLevel(float Level) const
{
	return ScaleRateCurve.GetValueAtLevel(Level);
}

int32 UAuraAbility_ArcaneSpike::GetNumArcaneShardsByLevel(float Level) const
{
	return NumArcaneShardsCurve.GetValueAtLevel(Level);
}
