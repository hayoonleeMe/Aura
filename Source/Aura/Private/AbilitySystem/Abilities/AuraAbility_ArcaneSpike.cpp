// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_ArcaneSpike.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_TargetDataUnderMouse.h"
#include "Aura/Aura.h"
#include "Types/DamageEffectParams.h"

UAuraAbility_ArcaneSpike::UAuraAbility_ArcaneSpike()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(AuraGameplayTags::Abilities_Offensive_ArcaneSpike);
	AbilityTags.AddTag(AuraGameplayTags::Abilities_NeedConfirm);	// 처음 실행한 뒤 Confirm 입력을 기다리는 어빌리티를 나타내는 GameplayTag
	BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_ClickToMove);
	BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_TryInteract);
	BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_Offensive);

	bNeedCursorTargetHitResult = false;
	bUseTriggeredEvent = false;
	
	DamageTypeTag = AuraGameplayTags::Damage_Type_Arcane;
	DebuffTag = AuraGameplayTags::Debuff_Enfeeble;
	MaxCastRange = 1400.f;
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
	FinalShardEffectiveRadius = ShardEffectiveRadius * GetScaleRateByLevel(GetAbilityLevel());
	TotalEffectiveRadius = FinalShardEffectiveRadius * 2.f;
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraAbility_ArcaneSpike::OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)
	{
		return;
	}

	const FHitResult& HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
	if (HitResult.bBlockingHit)
	{
		CachedTargetLocation = HitResult.ImpactPoint; 
	}

	const FTaggedCombatInfo TaggedCombatInfo = CombatInterface->GetTaggedCombatInfo(AuraGameplayTags::Abilities_Offensive_ArcaneSpike);
	check(TaggedCombatInfo.AnimMontage);

	// for Anim Montage Motion Warping
	CombatInterface->SetFacingTarget(CachedTargetLocation);

	PlayAttackMontage(TaggedCombatInfo.AnimMontage, true);
	WaitGameplayEvent(AuraGameplayTags::Event_Montage_ArcaneSpike);

	// 실제로 Arcane Shards를 스폰하고 나면 캐릭터 이동을 멈추도록 이동 관련 어빌리티를 취소한다.
	BlockMoveAbilities();
}

void UAuraAbility_ArcaneSpike::OnEventReceived(FGameplayEventData Payload)
{
	Super::OnEventReceived(Payload);
	
	SpawnArcaneShard();

	// 서버에서 Final Explosion Damage를 입힌 뒤 종료
	// bServerRespectsRemoteAbilityCancellation = true; 이므로 서버에서 데미지를 입히기 전에 종료되지 않도록 서버에서만 종료
	if (HasAuthority(&CurrentActivationInfo))
	{
		FinishAttack();
	}
}

float UAuraAbility_ArcaneSpike::GetRangeRadius() const
{
	return TotalEffectiveRadius;
}

void UAuraAbility_ArcaneSpike::SpawnArcaneShard() const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !HasAuthority(&CurrentActivationInfo))	// Only Spawn in Server
	{
		return;
	}
	
	const float AbilityLevel = GetAbilityLevel();
	
	// 범위 결정
	const FVector TargetLocation = ComputeValidTargetLocation(CachedTargetLocation);
	const float ScaleRate = GetScaleRateByLevel(AbilityLevel);

	FGameplayCueParameters CueParameters;
	CueParameters.RawMagnitude = ScaleRate;
	
	const int32 NumArcaneShards = GetNumArcaneShardsByLevel(AbilityLevel);
	TArray<FVector> TargetLocations;
	
	// Spawn Arcane Shard
	if (NumArcaneShards == 1)
	{
		TargetLocations.Add(TargetLocation);
		CueParameters.Location = TargetLocation;
		ASC->ExecuteGameplayCue(AuraGameplayTags::GameplayCue_ArcaneShard, CueParameters);
	}
	else
	{
		// TargetLocation을 기준으로 Shard의 중심까지 FinalShardEffectiveRadius인 원을 동일한 간격(Angle)으로 소환한다.
		const float RandAngle = FMath::RandRange(0.f, 180.f);
		const float Angle = 360.f / NumArcaneShards;
		for (int32 Index = 0; Index < NumArcaneShards; ++Index)
		{
			const FVector Offset = FVector::ForwardVector * FinalShardEffectiveRadius;
			FVector FinalTargetLocation = TargetLocation + Offset.RotateAngleAxis(RandAngle + Angle * Index, FVector::UpVector);
			TargetLocations.Add(FinalTargetLocation);
			CueParameters.Location = FinalTargetLocation;
			ASC->ExecuteGameplayCue(AuraGameplayTags::GameplayCue_ArcaneShard, CueParameters);
		}
	}

	// Arcane Shard 들에 의해 Damage를 입을 Enemies 결정
	TArray<AActor*> EnemiesToApplyDamage;
	for (const FVector& Location : TargetLocations)
	{
		TArray<AActor*> Enemies;
		UAuraBlueprintLibrary::GetEnemiesOverlappedByChannel(GetWorld(), Enemies, Location, FQuat::Identity, ECC_Target, FCollisionShape::MakeSphere(FinalShardEffectiveRadius));
		for (AActor* Enemy : Enemies)
		{
			EnemiesToApplyDamage.AddUnique(Enemy);
		}
	}

	// Apply Damage Effect
	FDamageEffectParams Params = MakeDamageEffectParams(nullptr);
	for (AActor* Enemy : EnemiesToApplyDamage)
	{
		Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Enemy);
		UAuraBlueprintLibrary::ApplyDamageEffect(Params);
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
