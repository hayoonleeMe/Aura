// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_FireBolt.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_TargetDataUnderMouse.h"
#include "Actor/AuraProjectile.h"
#include "Actor/PooledProjectile.h"
#include "Aura/Aura.h"
#include "Interface/ObjectPoolInterface.h"
#include "GameFramework/GameStateBase.h"

UAuraAbility_FireBolt::UAuraAbility_FireBolt()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(AuraGameplayTags::Abilities_Offensive_FireBolt);
	CancelAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_ClickToMove);
	CancelAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_TryInteract);
	BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_ClickToMove);
	BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_Offensive);
	bOverridePitch = true;
	DamageTypeTag = AuraGameplayTags::Damage_Type_Fire;
	DebuffTag = AuraGameplayTags::Debuff_Ignite;
	StartupInputTag = AuraGameplayTags::InputTag_LMB;
	SpreadAngle = 10.f;
}

FText UAuraAbility_FireBolt::GetDescription(int32 Level) const
{
	const float ScaledDamage = GetDamageByLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);

	const int32 NumFireBolts = GetNumFireBoltsByLevel(Level);

	const int32 DebuffChance = GetDebuffChanceByLevel(Level);

	FString PartStr = TEXT("<Default>Launches a bolt of fire, ");
	if (Level != 1)
	{
		PartStr = FString::Printf(TEXT("<Default>Launches </><Damage>%d </><Default>bolts of fire, "), NumFireBolts);
	}
	
	const FString RetStr = FString::Printf(TEXT(
		// Title
		"<Title>FIRE BOLT</>\n\n"

		// Level
		"<Default>Level: </><Level>%d</>\n"
		// ManaCost
		"<Default>ManaCost: </><ManaCost>%.1f</>\n"
		// Cooldown
		"<Default>Cooldown: </><Cooldown>%.1f</>\n\n"

		// NumProjectiles
		"%s"
		"exploding on impact and dealing </>"

		// Damage
		"<Damage>%.1f</><Default> fire damage. </>"

		// Debuff
		"<Default>This spell has a </><Percent>%d%% </><Default>chance to ignite the target on hit. "
		"Ignite burns the target for </><Time>4 </><Default>seconds, dealing fire damage every second "
		"equal to </><Percent>10%% </><Default>of this spell's damage. Can stack.</>"),

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

void UAuraAbility_FireBolt::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAuraDamageAbility::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 클라이언트의 Cursor HitResult와 CursorTarget을 항상 서버로 전달하기 위해 UAbilityTask_TargetDataUnderMouse 수행 
	if (UAbilityTask_TargetDataUnderMouse* AbilityTask = UAbilityTask_TargetDataUnderMouse::CreateTask(this))
	{
		AbilityTask->TargetDataUnderMouseSetDelegate.BindUObject(this, &ThisClass::OnTargetDataUnderMouseSet);
		AbilityTask->ReadyForActivation();
	}
}

void UAuraAbility_FireBolt::OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	const UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!AuraASC || !CombatInterface)
	{
		return;
	}
	
	// UAbilityTask_TargetDataUnderMouse에서 Cursor HitResult를 구하고 CursorTarget을 설정한다.
	const FHitResult& HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
	CachedImpactPoint = HitResult.ImpactPoint;
	
	const FTaggedCombatInfo TaggedCombatInfo = CombatInterface->GetTaggedCombatInfo(AuraGameplayTags::Abilities_Offensive_FireBolt);
	check(TaggedCombatInfo.AnimMontage);
	CachedCombatSocketName = TaggedCombatInfo.CombatSocketName;

	// for Anim Montage Motion Warping
	const FVector TargetLocation = AuraASC->CursorTargetWeakPtr.IsValid() ? AuraASC->CursorTargetWeakPtr->GetActorLocation() : CachedImpactPoint;
	CombatInterface->SetFacingTarget(TargetLocation);

	PlayAttackMontage(TaggedCombatInfo.AnimMontage, true);
	WaitGameplayEvent(AuraGameplayTags::Event_Montage_FireBolt);
}

void UAuraAbility_FireBolt::OnEventReceived(FGameplayEventData Payload)
{
	Super::OnEventReceived(Payload);
	SpawnFireBolts();
	FinishAttack();
}

int32 UAuraAbility_FireBolt::GetNumFireBoltsByLevel(float Level) const
{
	return NumFireBoltsCurve.GetValueAtLevel(Level);
}

void UAuraAbility_FireBolt::SpawnFireBolts() const
{
	check(ProjectileClass);

	const UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AuraASC || !IsValid(AvatarActor))
	{
		return;
	}
	
	const ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)
	{
		return;
	}
	
	const FVector CombatSocketLocation = CombatInterface->GetCombatSocketLocation(CachedCombatSocketName);
	const int32 NumFireBolts = GetNumFireBoltsByLevel(GetAbilityLevel());

	// Projectile 발사 방향 계산
	const FVector TargetLocation = AuraASC->CursorTargetWeakPtr.IsValid() ? AuraASC->CursorTargetWeakPtr->GetActorLocation() : CachedImpactPoint;
	const FVector CentralDirection = TargetLocation - AvatarActor->GetActorLocation();
	TArray<FVector> Directions;
	UAuraBlueprintLibrary::GetSpreadDirections(Directions, NumFireBolts, SpreadAngle, CentralDirection);

	// Enemy가 가까우면 항상 FireBolt가 적중하도록 SpawnLocation 보정
	FVector SpawnLocation = CombatSocketLocation;
	const FVector NearForwardLocation = AvatarActor->GetActorLocation() + AvatarActor->GetActorForwardVector() * 50.f;
	const FCollisionQueryParams QueryParams("", false, AvatarActor);
	if (GetWorld()->OverlapAnyTestByChannel(NearForwardLocation, FQuat::Identity, ECC_Target, FCollisionShape::MakeSphere(30.f), QueryParams))
	{
		SpawnLocation = NearForwardLocation;
	}

	// 각 방향으로 발사
	for (const FVector& Direction : Directions)
	{
		FRotator Rotation = Direction.Rotation();
		Rotation.Roll = 0.f;
		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		// Object Pooling instead of spawn actor
		if (IObjectPoolInterface* ObjectPoolInterface = Cast<IObjectPoolInterface>(GetWorld() ? GetWorld()->GetGameState() : nullptr))
		{
			if (const APooledProjectile* ProjectileCDO = ProjectileClass->GetDefaultObject<APooledProjectile>())
			{
				if (APooledProjectile* PooledProjectile = ObjectPoolInterface->SpawnFromPool<APooledProjectile>(ProjectileCDO->GetPooledActorType(), SpawnTransform, false))
				{
					// Projectile로 데미지를 입히기 위해 설정
					PooledProjectile->DamageEffectParams = MakeDamageEffectParams(nullptr);
					PooledProjectile->SetInUse(true);
				}
			}
		}	
	}
}
