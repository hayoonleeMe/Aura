// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_CastConfirmBase.h"

#include "AbilitySystemComponent.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "Actor/RangeDecalActor.h"
#include "Player/AuraPlayerController.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_TargetDataUnderMouse.h"
#include "Actor/BoxVolume.h"

UAuraAbility_CastConfirmBase::UAuraAbility_CastConfirmBase()
{
	MaxCastRange = 1400.f;
}

void UAuraAbility_CastConfirmBase::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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

void UAuraAbility_CastConfirmBase::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	// Caching
	if (const ABoxVolume* Volume = UAuraBlueprintLibrary::GetActorInWorldForTag<ABoxVolume>(GetWorld(), TEXT("PlayableVolume")))
	{
		PlayableBounds = Volume->GetVolumeBounds();
	}
}

FVector UAuraAbility_CastConfirmBase::ComputeValidTargetLocation(const FVector& TargetLocation) const
{
	if (const AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		// Target Location이 최대 사거리를 넘어가지 않도록 보정
		FVector FinalTargetLocation = TargetLocation;
		FVector StartLocation = AvatarActor->GetActorLocation();
		StartLocation.Z = FinalTargetLocation.Z;
		if (FVector::DistSquared(StartLocation, FinalTargetLocation) > MaxCastRange * MaxCastRange)
		{
			const FVector Direction = (FinalTargetLocation - StartLocation).GetSafeNormal();
			FinalTargetLocation = StartLocation + Direction * MaxCastRange;
		}

		// 스펠 범위가 벽에 막히지 않도록 보정
		const FVector SafeMin = PlayableBounds.Min + GetRangeRadius();
		const FVector SafeMax = PlayableBounds.Max - GetRangeRadius();

		FinalTargetLocation.X = FMath::Clamp(FinalTargetLocation.X, SafeMin.X, SafeMax.X);
		FinalTargetLocation.Y = FMath::Clamp(FinalTargetLocation.Y, SafeMin.Y, SafeMax.Y);

		// Floor와 겹침 방지
		FinalTargetLocation.Z = 0.1f;
		
		return FinalTargetLocation;
	}
	return FVector();
}

void UAuraAbility_CastConfirmBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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

void UAuraAbility_CastConfirmBase::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	// 처음 실행 후 추가 입력을 기다리는 중, 어빌리티 키를 추가 입력하거나 다른 어빌리티를 실행해 취소될 수 있다.
	if (CanBeCanceled())
	{
		HandleConfirmation();
	}
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UAuraAbility_CastConfirmBase::ShowRangeDecalActor(bool bShow)
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
			RangeDecalActor->ShowRangeDecal(true, GetRangeRadius());
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

void UAuraAbility_CastConfirmBase::OnLocalConfirmed()
{
	HandleConfirmation();

	// 이제 Firenado를 소환할 위치를 결정했으니 다른 어빌리티 실행을 막는다.
	SetShouldBlockOtherAbilities(true);

	// 현재 커서 위치에 Firenado를 소환하기 위해 TargetData 준비
	SetOrWaitTargetData();
}

void UAuraAbility_CastConfirmBase::HandleConfirmation()
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

void UAuraAbility_CastConfirmBase::SetOrWaitTargetData()
{
	if (UAbilityTask_TargetDataUnderMouse* AbilityTask = UAbilityTask_TargetDataUnderMouse::CreateTask(this))
	{
		AbilityTask->TargetDataUnderMouseSetDelegate.BindUObject(this, &ThisClass::OnTargetDataUnderMouseSet);
		AbilityTask->ReadyForActivation();
	}
}

void UAuraAbility_CastConfirmBase::BlockMoveAbilities() const
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayTagContainer Tags;
		Tags.AddTag(AuraGameplayTags::Abilities_ClickToMove);
		Tags.AddTag(AuraGameplayTags::Abilities_TryInteract);
		ASC->CancelAbilities(&Tags);
	}
}
