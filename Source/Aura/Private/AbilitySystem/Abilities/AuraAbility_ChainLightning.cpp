// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_ChainLightning.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_TargetDataUnderMouse.h"
#include "Aura/Aura.h"
#include "Interaction/CombatInterface.h"
#include "Types/DamageEffectParams.h"

UAuraAbility_ChainLightning::UAuraAbility_ChainLightning()
{
	MaxCastRange = 600.f;
}

FText UAuraAbility_ChainLightning::GetDescription(int32 Level) const
{
	const float ScaledDamage = GetDamageByLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	const int32 MaxChainCount = GetMaxChainCountByLevel(Level);
	const float ChainDamageRate = GetChainDamageRateByLevel(Level);
	const int32 ChainDamagePercent = ChainDamageRate * 100.f;

	const FString RetStr = FString::Printf(TEXT(
		// Title
		"<Title>CHAIN LIGHTNING</>\n\n"

		// Level
		"<Default>Level: </><Level>%d</>\n"
		// ManaCost
		"<Default>ManaCost: </><ManaCost>%.1f</>\n"
		// Cooldown
		"<Default>Cooldown: </><Cooldown>%.1f</>\n\n"

		// Description
		"<Default>Fires a bolt of lightning that strikes a target and nearby enemies, "
		"dealing </><Damage>%.1f</><Default> lightning damage with a chance to stun. "
		"The lightning chains up to </><Damage>%d</><Default> nearby enemies, "
		"dealing </><Percent>%d%%</><Default> of the previous damage to each chain.</>"),

		// Values
		Level,
		ManaCost,
		Cooldown,
		ScaledDamage,
		MaxChainCount,
		ChainDamagePercent
	);

	return FText::FromString(RetStr);
}

void UAuraAbility_ChainLightning::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		return;
	}

	const UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (!AuraASC)
	{
		return;
	}

	const ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(AuraASC->CursorTargetWeakPtr);
	if (AuraASC->CursorTargetWeakPtr.IsValid() && TargetCombatInterface && !TargetCombatInterface->IsDead())
	{
		// CursorTarget이 유효하면 계속해서 공격 수행
		ProcessAttack();
	}
	else
	{
		// 유효하지 않거나 죽었으면 TargetActor 결정
		if (UAbilityTask_TargetDataUnderMouse* AbilityTask = UAbilityTask_TargetDataUnderMouse::CreateTask(this))
		{
			AbilityTask->TargetDataUnderMouseSetDelegate.BindUObject(this, &ThisClass::OnTargetDataUnderMouseSet);
			AbilityTask->ReadyForActivation();
		}
	}
}

void UAuraAbility_ChainLightning::ProcessAttack()
{
	const UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!AuraASC || !CombatInterface)
	{
		return;
	}
	
	const FTaggedCombatInfo TaggedCombatInfo = CombatInterface->GetTaggedCombatInfo(FAuraGameplayTags::Get().Abilities_Offensive_ChainLightning);
	check(TaggedCombatInfo.AnimMontage);
	CachedCombatSocketName = TaggedCombatInfo.CombatSocketName;
	
	// for Anim Montage Motion Warping
    const FVector TargetLocation = AuraASC->CursorTargetWeakPtr.IsValid() ? AuraASC->CursorTargetWeakPtr->GetActorLocation() : CachedImpactPoint;
    CombatInterface->SetFacingTarget(TargetLocation);

	PlayAttackMontage(TaggedCombatInfo.AnimMontage, true);
	WaitGameplayEvent(FAuraGameplayTags::Get().Event_Montage_ChainLightning);
}

void UAuraAbility_ChainLightning::OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (!AuraASC)
	{
		return;
	}

	// CursorTarget Caching
	const FHitResult& HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
	const ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(HitResult.GetActor());
	AuraASC->CursorTargetWeakPtr = TargetCombatInterface && !TargetCombatInterface->IsDead() ? HitResult.GetActor() : nullptr;
	CachedImpactPoint = HitResult.ImpactPoint;
	
	ProcessAttack();
}

void UAuraAbility_ChainLightning::OnEventReceived(FGameplayEventData Payload)
{
	CastLightningBeam();
	FinishAttack();
}

void UAuraAbility_ChainLightning::CastLightningBeam() const
{
	const UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AuraASC || !IsValid(AvatarActor) || !AvatarActor->HasAuthority())	// Only Spawn in Server
	{
		return;
	}
	
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)
	{
		return;
	}

	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

	AActor* FirstTargetActor = AuraASC->CursorTargetWeakPtr.Get();
	FVector TargetLocation = FirstTargetActor ? FirstTargetActor->GetActorLocation() : CachedImpactPoint;
	const FVector CombatSocketLocation = CombatInterface->GetCombatSocketLocation(CachedCombatSocketName);
	const FVector StartLocation = FirstTargetActor ? CombatSocketLocation : AvatarActor->GetActorLocation();

	FRotator Rotation = (TargetLocation - StartLocation).GetSafeNormal().Rotation();
	Rotation.Roll = 0.f;
	if (!FirstTargetActor)
	{
		// CursorTarget이 없다면 바닥과 평행하게 발사
		Rotation.Pitch = 0.f;
	}
	// Spell의 최대 시전 사거리 적용
	TargetLocation = StartLocation + Rotation.Vector() * MaxCastRange;

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, CombatSocketLocation, TargetLocation, ECC_Projectile))
	{
		// 처음 플레이어 무기에서 발사되는 Beam의 막히는 경우 고려
		if (HitResult.GetActor() != FirstTargetActor)
		{
			const ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(HitResult.GetActor());
			FirstTargetActor = TargetCombatInterface && !TargetCombatInterface->IsDead() ? HitResult.GetActor() : nullptr;
		}
		// 최종 Target 결정
		TargetLocation = FirstTargetActor ? FirstTargetActor->GetActorLocation() : HitResult.ImpactPoint;
	}

	// Spawn Beam from Player to Target
	UAuraBlueprintLibrary::ExecuteGameplayCue(AvatarActor, GameplayTags.GameplayCue_LightningBeam, TargetLocation);

	// First Target이 없다면 Damage를 입히지 않기 위해 Early Return
	if (!IsValid(FirstTargetActor))
	{
		return;
	}

	// 첫 Target 주변 Enemy 선택
	TArray<AActor*> PossibleEnemies;
	const float InitialChainRadius = GetInitialChainRadiusByLevel(GetAbilityLevel());
	UAuraBlueprintLibrary::GetEnemiesOverlappedByChannel(GetWorld(), PossibleEnemies, FirstTargetActor->GetActorLocation(), FQuat::Identity, ECC_Target, FCollisionShape::MakeSphere(InitialChainRadius));

	AActor* CurrentTarget = FirstTargetActor;
	TArray<AActor*> SelectedActors;
	SelectedActors.Add(CurrentTarget);

	// CurrentTarget에서 가장 가까운 Target을 연쇄적으로 결정
	const int32 MaxChainCount = GetMaxChainCountByLevel(GetAbilityLevel());
	for (int32 Index = 0; Index < MaxChainCount; ++Index)
	{
		float NearestDistSquared = TNumericLimits<float>::Max();
		AActor* NearestTarget = nullptr;
		
		for (AActor* Enemy : PossibleEnemies)
		{
			if (CurrentTarget && Enemy && !SelectedActors.Contains(Enemy))
			{
				const float DistSquared = FVector::DistSquared(CurrentTarget->GetActorLocation(), Enemy->GetActorLocation());
				if (DistSquared < NearestDistSquared)
				{
					NearestDistSquared = DistSquared;
					NearestTarget = Enemy;
				}
			}
		}

		if (!NearestTarget)
		{
			break;
		}
		
		// Spawn Beam CurrentTarget To NearestTarget
		UAuraBlueprintLibrary::ExecuteGameplayCue(CurrentTarget, GameplayTags.GameplayCue_LightningBeam, NearestTarget->GetActorLocation());
		
		SelectedActors.Add(NearestTarget);
		CurrentTarget = NearestTarget; 
	}

	// Apply Damage
	float CurrentDamage = GetDamageByLevel(GetAbilityLevel());
	const float ChainDamageRate = GetChainDamageRateByLevel(GetAbilityLevel());
	
	for (AActor* Actor : SelectedActors)
	{
		FDamageEffectParams DamageEffectParams;
		MakeDamageEffectParams(DamageEffectParams, Actor);
		if (Actor != FirstTargetActor)
		{
			// 연쇄될 때마다 데미지 감소
			CurrentDamage *= ChainDamageRate;
			DamageEffectParams.BaseDamage = CurrentDamage;
		}
		UAuraBlueprintLibrary::ApplyDamageEffect(DamageEffectParams);
	}
}

float UAuraAbility_ChainLightning::GetInitialChainRadiusByLevel(float Level) const
{
	return InitialChainRadiusCurve.GetValueAtLevel(Level);
}

int32 UAuraAbility_ChainLightning::GetMaxChainCountByLevel(float Level) const
{
	return MaxChainCountCurve.GetValueAtLevel(Level);
}

float UAuraAbility_ChainLightning::GetChainDamageRateByLevel(float Level) const
{
	return ChainDamageRateCurve.GetValueAtLevel(Level);
}
