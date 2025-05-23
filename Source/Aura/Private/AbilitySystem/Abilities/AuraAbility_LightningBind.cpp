// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_LightningBind.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/LightningBindActor.h"
#include "Interface/CombatInterface.h"

UAuraAbility_LightningBind::UAuraAbility_LightningBind()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(AuraGameplayTags::Abilities_Offensive_LightningBind);
	AbilityTags.AddTag(AuraGameplayTags::Abilities_NeedConfirm);	// 처음 실행한 뒤 Confirm 입력을 기다리는 어빌리티를 나타내는 GameplayTag
	BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_ClickToMove);
	BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_TryInteract);
	BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_Offensive);

	bNeedCursorTargetHitResult = false;
	bUseTriggeredEvent = false;
	UnlockRequiredLevel = 2;
	
	DamageTypeTag = AuraGameplayTags::Damage_Type_Lightning;
	DebuffTag = AuraGameplayTags::Debuff_Stun;
	DebuffChanceCurve.SetValue(100.f);
	EffectiveRadius = 80.f;
	DamageInterval = 1.f;
	MaxCastRange = 1000.f;

	/* Stack */
	bUseSpellStack = true;
}

FText UAuraAbility_LightningBind::GetDescription(int32 Level) const
{
	// @todo
	return Super::GetDescription(Level);
}

void UAuraAbility_LightningBind::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	FinalEffectiveRadius = EffectiveRadius * GetScaleRateByLevel(GetAbilityLevel());
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraAbility_LightningBind::OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
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

	const FTaggedCombatInfo TaggedCombatInfo = CombatInterface->GetTaggedCombatInfo(AuraGameplayTags::Abilities_Offensive_LightningBind);
	check(TaggedCombatInfo.AnimMontage);

	// for Anim Montage Motion Warping
	CombatInterface->SetFacingTarget(CachedTargetLocation);

	PlayAttackMontage(TaggedCombatInfo.AnimMontage, true);
	WaitGameplayEvent(AuraGameplayTags::Event_Montage_LightningBind);

	// Confirm 이후 실제로 기능을 수행할 때 캐릭터 이동을 멈추도록 이동 관련 어빌리티를 취소한다.
	BlockMoveAbilities();
}

void UAuraAbility_LightningBind::OnEventReceived(FGameplayEventData Payload)
{
	Super::OnEventReceived(Payload);

	UpdateSpellStack(-1);
	SpawnLightningBind();

	// 서버에서 Final Explosion Damage를 입힌 뒤 종료
	// bServerRespectsRemoteAbilityCancellation = true; 이므로 서버에서 데미지를 입히기 전에 종료되지 않도록 서버에서만 종료
	if (HasAuthority(&CurrentActivationInfo))
	{
		FinishAttack();
	}
}

float UAuraAbility_LightningBind::GetRangeRadius() const
{
	return FinalEffectiveRadius;
}

void UAuraAbility_LightningBind::SpawnLightningBind()
{
	if (GetWorld())
	{
		const FVector FinalTargetLocation = ComputeValidTargetLocation(CachedTargetLocation);
		const FTransform SpawnTransform(FinalTargetLocation);
		const float AbilityLevel = GetAbilityLevel();
		
		if (ALightningBindActor* LightningBindActor = GetWorld()->SpawnActorDeferred<ALightningBindActor>(ALightningBindActor::StaticClass(), SpawnTransform))
		{
			FDamageEffectParams DamageEffectParams = MakeDamageEffectParams(nullptr);
			DamageEffectParams.DebuffChance = DebuffChanceCurve.GetValue();
			DamageEffectParams.DebuffDuration = DamageInterval;
			
			LightningBindActor->Initialize(FinalEffectiveRadius, GetScaleRateByLevel(AbilityLevel), DamageInterval, GetTotalDamageCountByLevel(AbilityLevel), FinalTargetLocation, DamageEffectParams, GetAbilitySystemComponentFromActorInfo());
			LightningBindActor->FinishSpawning(SpawnTransform);
		}
	}
}

float UAuraAbility_LightningBind::GetScaleRateByLevel(float Level) const
{
	return ScaleRateCurve.GetValueAtLevel(Level);
}

float UAuraAbility_LightningBind::GetTotalDamageCountByLevel(float Level) const
{
	return TotalDamageCountCurve.GetValueAtLevel(Level);
}
