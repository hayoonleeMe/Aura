// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_Firenado.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_TargetDataUnderMouse.h"
#include "Actor/FirenadoActor.h"
#include "Actor/RangeDecalActor.h"
#include "Interface/CombatInterface.h"
#include "Player/AuraPlayerController.h"

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
	MaxCastRange = 1400.f;
	EffectiveRadius = 120.f;
	ExplosionEffectiveRadius = 120.f;
	DamageInterval = 0.6f;
	TotalDamageCount = 6;
}

void UAuraAbility_Firenado::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	// 처음 실행하고 어빌리티 키를 추가 입력하면 어빌리티를 취소
	const bool bAlreadyConfirmed = IsBlockingOtherAbilities();
	if (!bAlreadyConfirmed && bAlreadyPressed)
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		return;
	}

	bAlreadyPressed = true;
}

FText UAuraAbility_Firenado::GetDescription(int32 Level) const
{
	// TODO
	return Super::GetDescription(Level);
}

FVector UAuraAbility_Firenado::ComputeValidTargetLocation(const FVector& TargetLocation) const
{
	/**
	 *	@todo
	 *	Firenado를 소환할 수 없는 곳을 소환할 수 있는 곳으로 보정?
	 *	또는 소환할 수 없는 곳을 다른 색으로 표시하기
	 */
	if (const AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		FVector FinalTargetLocation = TargetLocation;
		FVector StartLocation = AvatarActor->GetActorLocation();
		StartLocation.Z = FinalTargetLocation.Z;
		if (FVector::DistSquared(StartLocation, FinalTargetLocation) > MaxCastRange * MaxCastRange)
		{
			const FVector Direction = (FinalTargetLocation - StartLocation).GetSafeNormal();
			FinalTargetLocation = StartLocation + Direction * MaxCastRange;
		}
		return FinalTargetLocation;	
	}
	return FVector();
}

void UAuraAbility_Firenado::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bAlreadyPressed = false;
	
	// 처음 실행 후 다른 어빌리티를 실행하면 취소되도록 Block을 해제한다. 
	SetShouldBlockOtherAbilities(false);

	// 처음 실행 후 추가 입력을 기다리는 것은 로컬에서만 수행해도 충분하다.
	if (IsLocallyControlled())
	{
		ShowRangeDecalActor(true);
		
		if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(GetCurrentActorInfo()->PlayerController))
		{
			// 더 높은 우선순위를 가지는 Waiting Confirmation Context를 추가해 Spell Confirmation을 수행하는 LMB 입력을 받는다.
			AuraPC->EnableWaitingConfirmationInput(true);

			// 커서로 액터를 Highlight하는 것을 방지
			AuraPC->EnableHighlight(false);

			// 스펠 범위를 표시하기 위해 ECC_Floor로 CursorTrace를 수행하도록 설정
			AuraPC->SetNeedCursorTarget(false);
		}

		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			// LMB 입력을 포착하기 위해 콜백 함수를 등록한다.
			// 특정 InputAction에 의한 입력이 들어오면 브로드캐스트된다.
			// @see AAuraPlayerController::OnConfirmSpellActionStarted()
			ASC->GenericLocalConfirmCallbacks.AddDynamic(this, &ThisClass::OnLocalConfirmed);	
		}
	}
	else
	{
		// 서버 측 RemoteRole이 Autonomous Proxy인 인스턴스에서 클라이언트 측의 TargetData를 기다린다.
		// 로컬 기기에선 현재 마우스 커서 위치를 전달한다.
		SetOrWaitTargetData();
	}
}

void UAuraAbility_Firenado::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	// 처음 실행 후 추가 입력을 기다리는 중, 어빌리티 키를 추가 입력하거나 다른 어빌리티를 실행해 취소될 수 있다.
	if (CanBeCanceled())
	{
		HandleConfirmation();
	}
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UAuraAbility_Firenado::ShowRangeDecalActor(bool bShow)
{
	if (bShow)
	{
		if (!IsValid(RangeDecalActor))
		{
			RangeDecalActor = GetWorld()->SpawnActor<ARangeDecalActor>(RangeDecalActorClass);
			RangeDecalActor->Initialize(this);
		}
		if (IsValid(RangeDecalActor))
		{
			// 스펠의 범위를 RangeDecalActor로 표시한다.
			RangeDecalActor->ShowRangeDecal(true, EffectiveRadius * GetScaleRateByLevel(GetAbilityLevel()));
		}
	}
	else
	{
		if (IsValid(RangeDecalActor))
		{
			RangeDecalActor->ShowRangeDecal(false);
		}
	}
}

void UAuraAbility_Firenado::OnLocalConfirmed()
{
	HandleConfirmation();

	// 이제 Firenado를 소환할 위치를 결정했으니 다른 어빌리티 실행을 막는다.
	SetShouldBlockOtherAbilities(true);

	// 현재 커서 위치에 Firenado를 소환하기 위해 TargetData 준비
	SetOrWaitTargetData();
}

void UAuraAbility_Firenado::HandleConfirmation()
{
	ShowRangeDecalActor(false);

	if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(GetCurrentActorInfo()->PlayerController))
	{
		// Remove Waiting Confirmation Input Mapping Context
		// 더 높은 우선순위를 가지는 Waiting Confirmation Context를 추가해 Spell Confirmation을 수행하는 LMB 입력을 받는다.
		AuraPC->EnableWaitingConfirmationInput(false);

		// 다시 커서로 액터를 Highlight 할 수 있도록 되돌린다.
		AuraPC->EnableHighlight(true);

		// 다시 원래대로 ECC_Target으로 Cursor Trace를 수행하도록 되돌린다.
		AuraPC->SetNeedCursorTarget(true);
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (ASC->GenericLocalConfirmCallbacks.IsAlreadyBound(this, &ThisClass::OnLocalConfirmed))
		{
			ASC->GenericLocalConfirmCallbacks.RemoveDynamic(this, &ThisClass::OnLocalConfirmed);	
		}
	}
}

void UAuraAbility_Firenado::SetOrWaitTargetData()
{
	if (UAbilityTask_TargetDataUnderMouse* AbilityTask = UAbilityTask_TargetDataUnderMouse::CreateTask(this))
	{
		AbilityTask->TargetDataUnderMouseSetDelegate.BindUObject(this, &ThisClass::OnTargetDataUnderMouseSet);
		AbilityTask->ReadyForActivation();
	}
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
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayTagContainer Tags;
		Tags.AddTag(AuraGameplayTags::Abilities_ClickToMove);
		Tags.AddTag(AuraGameplayTags::Abilities_TryInteract);
		ASC->CancelAbilities(&Tags);
	}
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

void UAuraAbility_Firenado::SpawnFirenado()
{
	if (GetWorld())
	{
		FVector FinalTargetLocation = ComputeValidTargetLocation(CachedTargetLocation);
		FinalTargetLocation.Z = 0.1f;	// Floor와 겹침 방지
		const FTransform SpawnTransform(FinalTargetLocation);
		const float AbilityLevel = GetAbilityLevel();
		
		if (AFirenadoActor* FirenadoActor = GetWorld()->SpawnActorDeferred<AFirenadoActor>(AFirenadoActor::StaticClass(), SpawnTransform))
		{
			FirenadoActor->Initialize(EffectiveRadius, ExplosionEffectiveRadius, GetScaleRateByLevel(AbilityLevel), GetDamageByLevel(AbilityLevel), GetFinalDamageRateByLevel(AbilityLevel), DamageInterval, TotalDamageCount, FinalTargetLocation, MakeDamageEffectParams(nullptr), GetAbilitySystemComponentFromActorInfo());
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
