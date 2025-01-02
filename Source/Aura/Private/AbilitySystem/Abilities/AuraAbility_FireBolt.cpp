// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_FireBolt.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_TargetDataUnderMouse.h"
#include "Actor/AuraProjectile.h"

FText UAuraAbility_FireBolt::GetDescription(int32 Level) const
{
	const float ScaledDamage = GetDamageByLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);

	checkf(NumFireBoltsCurve, TEXT("Need to set NumFireBoltsCurve"));
	const int32 NumFireBolts = NumFireBoltsCurve->GetFloatValue(Level);

	FString RetStr;
	if (Level == 1)
	{
		RetStr = FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Level
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			"<Default>Launches a bolt of fire, "
			"exploding on impact and dealing: </>"

			// Damage
			"<Damage>%.1f</><Default> fire damage with"
			" a chance to burn</>"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			ScaledDamage
		);
	}
	else
	{
		RetStr = FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Level
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// NumProjectiles
			"<Default>Launches %d bolts of fire, "
			"exploding on impact and dealing: </>"

			// Damage
			"<Damage>%.1f</><Default> fire damage with"
			" a chance to burn</>"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			NumFireBolts,
			ScaledDamage
		);
	}

	return FText::FromString(RetStr);
}

void UAuraAbility_FireBolt::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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

void UAuraAbility_FireBolt::OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)
	{
		return;
	}

	// Caching
	const FHitResult& HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
	CachedTargetLocation = HitResult.ImpactPoint;
	
	const FTaggedCombatInfo TaggedCombatInfo = CombatInterface->GetTaggedCombatInfo(FAuraGameplayTags::Get().Abilities_Offensive_FireBolt);
	check(TaggedCombatInfo.AnimMontage);
	CachedCombatSocketName = TaggedCombatInfo.CombatSocketName;

	// for Anim Montage Motion Warping
	CombatInterface->SetFacingTarget(CachedTargetLocation);

	PlayAttackMontage(TaggedCombatInfo.AnimMontage, true);
	WaitGameplayEvent(FAuraGameplayTags::Get().Event_Montage_FireBolt);
}

void UAuraAbility_FireBolt::OnEventReceived(FGameplayEventData Payload)
{
	const ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)
	{
		return;
	}
	
	const FVector CombatSocketLocation = CombatInterface->GetCombatSocketLocation(CachedCombatSocketName);
	SpawnFireBolts(CachedTargetLocation, CombatSocketLocation);

	FinishAttack();
}

void UAuraAbility_FireBolt::SpawnFireBolts(const FVector& TargetLocation, const FVector& CombatSocketLocation) const
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor) || !AvatarActor->HasAuthority())	// Only Spawn in Server
	{
		return;
	}
	check(ProjectileClass);

	checkf(NumFireBoltsCurve, TEXT("Need to set NumFireBoltsCurve"));
	const int32 NumFireBolts = NumFireBoltsCurve->GetFloatValue(GetAbilityLevel());

	// Projectile 발사 방향 계산
	const FVector StartLocation = AvatarActor->GetActorLocation();
	const FVector CentralDirection = TargetLocation - StartLocation;
	TArray<FVector> Directions;
	UAuraBlueprintLibrary::GetSpreadDirections(Directions, NumFireBolts, 10.f /* TODO : Parameterize */, CentralDirection);

	// 각 방향으로 발사
	for (const FVector& Direction : Directions)
	{
		FRotator Rotation = Direction.Rotation();
		Rotation.Roll = 0.f;
		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}

		// CombatSocket에서 Projectile 발사
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(CombatSocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		AActor* OwningActor = GetOwningActorFromActorInfo();
		if (AAuraProjectile* AuraProjectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, OwningActor, Cast<APawn>(OwningActor), ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
		{
			// Projectile로 데미지를 입히기 위해 설정
			MakeDamageEffectParams(AuraProjectile->DamageEffectParams, nullptr);
			AuraProjectile->FinishSpawning(SpawnTransform);
		}	
	}
}
