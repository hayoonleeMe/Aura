// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_FireBolt.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_TargetDataUnderMouse.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

UAuraAbility_FireBolt::UAuraAbility_FireBolt()
{
	SpreadAngle = 20.f;
	MinHomingAcceleration = 1000.f;
	MaxHomingAcceleration = 3200.f;
}

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

	const UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (!AuraASC)
	{
		return;
	}
	
	if (!AuraASC->CursorTargetWeakPtr.IsValid())
	{
		// TargetActor를 결정
		if (UAbilityTask_TargetDataUnderMouse* AbilityTask = UAbilityTask_TargetDataUnderMouse::CreateTask(this))
		{
			AbilityTask->TargetDataUnderMouseSetDelegate.BindUObject(this, &ThisClass::OnTargetDataUnderMouseSet);
			AbilityTask->ReadyForActivation();
		}
	}
	else
	{
		ProcessAttack();
	}
}

void UAuraAbility_FireBolt::ProcessAttack()
{
	const UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!AuraASC || !CombatInterface)
	{
		return;
	}
	
	const FTaggedCombatInfo TaggedCombatInfo = CombatInterface->GetTaggedCombatInfo(FAuraGameplayTags::Get().Abilities_Offensive_FireBolt);
	check(TaggedCombatInfo.AnimMontage);
	CachedCombatSocketName = TaggedCombatInfo.CombatSocketName;

	// for Anim Montage Motion Warping
	const FVector TargetLocation = AuraASC->CursorTargetWeakPtr.IsValid() ? AuraASC->CursorTargetWeakPtr->GetActorLocation() : CachedImpactPoint;
	CombatInterface->SetFacingTarget(TargetLocation);

	PlayAttackMontage(TaggedCombatInfo.AnimMontage, true);
	WaitGameplayEvent(FAuraGameplayTags::Get().Event_Montage_FireBolt);
}

void UAuraAbility_FireBolt::OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (!AuraASC)
	{
		return;
	}

	// Caching
	const FHitResult& HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
	if (HitResult.GetActor() && HitResult.GetActor()->Implements<UCombatInterface>())
	{
		AuraASC->CursorTargetWeakPtr = HitResult.GetActor();
	}
	CachedImpactPoint = HitResult.ImpactPoint;
	
	ProcessAttack();
}

void UAuraAbility_FireBolt::OnEventReceived(FGameplayEventData Payload)
{
	SpawnFireBolts();
	FinishAttack();
}

void UAuraAbility_FireBolt::SpawnFireBolts() const
{
	check(ProjectileClass);

	const UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AuraASC || !IsValid(AvatarActor) || !AvatarActor->HasAuthority())	// Only Spawn in Server
	{
		return;
	}
	
	const ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)
	{
		return;
	}
	const FVector CombatSocketLocation = CombatInterface->GetCombatSocketLocation(CachedCombatSocketName);

	checkf(NumFireBoltsCurve, TEXT("Need to set NumFireBoltsCurve"));
	const int32 NumFireBolts = NumFireBoltsCurve->GetFloatValue(GetAbilityLevel());

	// Projectile 발사 방향 계산
	const FVector TargetLocation = AuraASC->CursorTargetWeakPtr.IsValid() ? AuraASC->CursorTargetWeakPtr->GetActorLocation() : CachedImpactPoint;
	const FVector StartLocation = AvatarActor->GetActorLocation();
	const FVector CentralDirection = TargetLocation - StartLocation;
	TArray<FVector> Directions;
	UAuraBlueprintLibrary::GetSpreadDirections(Directions, NumFireBolts, SpreadAngle, CentralDirection);

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

			// Cursor로 선택한 TargetActor가 있다면 Homing
			if (AuraASC->CursorTargetWeakPtr.IsValid() && AuraASC->CursorTargetWeakPtr->Implements<UCombatInterface>())
			{
				AuraProjectile->ProjectileMovementComponent->bIsHomingProjectile = true;
				AuraProjectile->ProjectileMovementComponent->HomingAccelerationMagnitude = FMath::RandRange(MinHomingAcceleration, MaxHomingAcceleration);
				AuraProjectile->ProjectileMovementComponent->HomingTargetComponent = AuraASC->CursorTargetWeakPtr->GetRootComponent();

				// Target이 죽으면 FireBolt Self Destroy 
				if (ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(AuraASC->CursorTargetWeakPtr))
				{
					TargetCombatInterface->GetOnCharacterDeadDelegate()->AddDynamic(AuraProjectile, &AAuraProjectile::OnHomingTargetDead);
				}
			}
			
			AuraProjectile->FinishSpawning(SpawnTransform);
		}	
	}
}
