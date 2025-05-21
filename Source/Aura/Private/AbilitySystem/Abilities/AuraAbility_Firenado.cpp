// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_Firenado.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/FirenadoActor.h"
#include "Interface/CombatInterface.h"

UAuraAbility_Firenado::UAuraAbility_Firenado()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(AuraGameplayTags::Abilities_Offensive_Firenado);
	AbilityTags.AddTag(AuraGameplayTags::Abilities_NeedConfirm);	// 처음 실행한 뒤 Confirm 입력을 기다리는 어빌리티를 나타내는 GameplayTag
	BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_ClickToMove);
	BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_TryInteract);
	BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_Offensive);

	bNeedCursorTargetHitResult = false;
	bUseTriggeredEvent = false;
	UnlockRequiredLevel = 3;
	
	DamageTypeTag = AuraGameplayTags::Damage_Type_Fire;
	EffectiveRadius = 120.f;
	ExplosionEffectiveRadius = 120.f;
	DamageInterval = 0.6f;
	TotalDamageCount = 6;
}

FText UAuraAbility_Firenado::GetDescription(int32 Level) const
{
    const float ManaCost = GetManaCost(Level);
    const float Cooldown = GetCooldown(Level);
	
    const float DamagePerInterval = GetDamageByLevel(Level);
    const float FinalDamageRate = GetFinalDamageRateByLevel(Level);
    const float FinalExplosionDamage = DamagePerInterval * FinalDamageRate;

    const int32 DebuffChance = GetDebuffChanceByLevel(Level);

    const FString RetStr = FString::Printf(TEXT(
        // Title
        "<Title>FIRENADO</>\n\n"

        // Level
        "<Default>Level: </><Level>%d</>\n"
        // ManaCost
        "<Default>ManaCost: </><ManaCost>%.1f</>\n"
        // Cooldown
        "<Default>Cooldown: </><Cooldown>%.1f</>\n\n"

        // Desc
        "<Default>Summons a devastating Firenado that strikes enemies </><Damage>%d</><Default> times. "
		"Each strike inflicts </><Damage>%.1f</><Default> fire damage every </><Time>%.1f</><Default> seconds to enemies within the vortex. "
		"Upon dissipating, the Firenado explodes for a final </><Damage>%.1f</><Default> fire damage to enemies within the explosion.</>\n\n"

        // Debuff
		"<Default>Each strike from the Firenado has a </><Percent>%d%% </><Default>chance to ignite the target. "
		"Ignite burns the target for </><Time>4 </><Default>seconds, dealing fire damage every second "
		"equal to </><Percent>10%% </><Default>of eacg damage. Can stack.</>"),

        // Values
        Level,
        ManaCost,
        Cooldown,
        TotalDamageCount,
        DamagePerInterval,
        DamageInterval,
        FinalExplosionDamage,
        DebuffChance
    );

    return FText::FromString(RetStr);
}

void UAuraAbility_Firenado::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	const float ScaleRate = GetScaleRateByLevel(GetAbilityLevel());
	FinalEffectiveRadius = EffectiveRadius * ScaleRate;
	FinalExplosionEffectiveRadius = ExplosionEffectiveRadius * ScaleRate;
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);	
}

void UAuraAbility_Firenado::OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
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

	const FTaggedCombatInfo TaggedCombatInfo = CombatInterface->GetTaggedCombatInfo(AuraGameplayTags::Abilities_Offensive_Firenado);
	check(TaggedCombatInfo.AnimMontage);

	// for Anim Montage Motion Warping
	CombatInterface->SetFacingTarget(CachedTargetLocation);

	PlayAttackMontage(TaggedCombatInfo.AnimMontage, true);
	WaitGameplayEvent(AuraGameplayTags::Event_Montage_Firenado);

	// 실제로 Firenado를 스폰하고 나면 캐릭터 이동을 멈추도록 이동 관련 어빌리티를 취소한다.
	BlockMoveAbilities();
}

void UAuraAbility_Firenado::OnEventReceived(FGameplayEventData Payload)
{
	Super::OnEventReceived(Payload);
	
	SpawnFirenado();

	// 서버에서 Final Explosion Damage를 입힌 뒤 종료
	// bServerRespectsRemoteAbilityCancellation = true; 이므로 서버에서 데미지를 입히기 전에 종료되지 않도록 서버에서만 종료
	if (HasAuthority(&CurrentActivationInfo))
	{
		FinishAttack();
	}
}

float UAuraAbility_Firenado::GetRangeRadius() const
{
	return FinalEffectiveRadius;
}

void UAuraAbility_Firenado::SpawnFirenado()
{
	if (GetWorld())
	{
		const FVector FinalTargetLocation = ComputeValidTargetLocation(CachedTargetLocation);
		const FTransform SpawnTransform(FinalTargetLocation);
		const float AbilityLevel = GetAbilityLevel();
		
		if (AFirenadoActor* FirenadoActor = GetWorld()->SpawnActorDeferred<AFirenadoActor>(AFirenadoActor::StaticClass(), SpawnTransform))
		{
			FirenadoActor->Initialize(FinalEffectiveRadius, FinalExplosionEffectiveRadius, GetScaleRateByLevel(AbilityLevel), GetDamageByLevel(AbilityLevel), GetFinalDamageRateByLevel(AbilityLevel), DamageInterval, TotalDamageCount, FinalTargetLocation, MakeDamageEffectParams(nullptr), GetAbilitySystemComponentFromActorInfo());
			FirenadoActor->FinishSpawning(SpawnTransform);
		}
	}
}

float UAuraAbility_Firenado::GetScaleRateByLevel(float Level) const
{
	return ScaleRateCurve.GetValueAtLevel(Level);
}

float UAuraAbility_Firenado::GetFinalDamageRateByLevel(float Level) const
{
	return FinalDamageRateCurve.GetValueAtLevel(Level);
}
