// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_TryInteract.h"

#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_MoveAlongNavPath.h"
#include "Interface/InteractionInterface.h"
#include "Player/AuraPlayerController.h"

UAuraAbility_TryInteract::UAuraAbility_TryInteract()
{
	AbilityTags.AddTag(AuraGameplayTags::Abilities_TryInteract);
	
	CancelAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_ClickToMove);
	// 어빌리티를 실행하면 현재 실행 중인 Confirmation을 기다리는 어빌리티를 취소한다.
	CancelAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_NeedConfirm);
	
	StartupInputTag = AuraGameplayTags::InputTag_Interact;
	InitialArriveAcceptanceRadius = ArriveAcceptanceRadius;
}

bool UAuraAbility_TryInteract::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	const APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (!AvatarPawn)
	{
		return false;
	}
	const AAuraPlayerController* AuraPC = AvatarPawn->GetController<AAuraPlayerController>();
	if (!AuraPC)
	{
		return false;
	}
	const IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(AuraPC->GetTargetActorFromCursor());
	if (!InteractionInterface || !InteractionInterface->CanTryInteract())
	{
		return false;
	}
	return true;
}

void UAuraAbility_TryInteract::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// InstancingPolicy가 InstancedPerActor이므로 Activate할 때마다 데이터를 초기화
	TargetActor = nullptr;
	ArriveAcceptanceRadius = InitialArriveAcceptanceRadius;

	Pawn = CastChecked<APawn>(GetAvatarActorFromActorInfo());
	if (const AAuraPlayerController* AuraPC = Pawn->GetController<AAuraPlayerController>())
	{
		TargetActor = AuraPC->GetTargetActorFromCursor();
	}
	const IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(TargetActor);

	if (!IsValid(TargetActor) || !InteractionInterface || !InteractionInterface->CanTryInteract())
	{
		K2_EndAbility();
		return;
	}

	// ArriveAcceptanceRadius 업데이트
	if (InteractionInterface->GetOverrideArriveAcceptanceRadius() > 0.f)
	{
		ArriveAcceptanceRadius = InteractionInterface->GetOverrideArriveAcceptanceRadius();
	}

	const FVector PawnLocation = Pawn->GetNavAgentLocation();
	const FVector TargetActorLocation = UAuraBlueprintLibrary::GetActorFeetLocation(TargetActor);
	
	// 충분히 가까우면 바로 상호작용
	const float SquaredDistToDest = (PawnLocation - TargetActorLocation).SquaredLength();
	if (SquaredDistToDest <= ArriveAcceptanceRadius * ArriveAcceptanceRadius)
	{
		Interact();
		return;
	}

	// 경로 계산
	if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(Pawn, PawnLocation, TargetActorLocation))
	{
		PathIndex = 0;
		NavPaths = MoveTemp(NavPath->PathPoints);
		if (NavPaths.IsEmpty())
		{
			K2_EndAbility();
			return;
		}
	}

	// AbilityTask_ClickToMove를 생성해 TargetActor까지 이동
	if (UAbilityTask_MoveAlongNavPath* Task = UAbilityTask_MoveAlongNavPath::CreateTask(this))
	{
		// MoveToDestination 함수 등록
		Task->MoveToDestinationDelegate.BindUObject(this, &ThisClass::MoveToDestination);
	
		// TargetActor에 도착하면 상호작용 후 Ability 종료
		Task->OnArrivedDelegate.BindLambda([this]()
		{
			Interact();
			K2_EndAbility();
		});
		Task->ReadyForActivation();
	}
}

void UAuraAbility_TryInteract::Interact()
{
	if (IsValid(TargetActor))
	{
		if (IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(TargetActor))
		{
			InteractionInterface->Interact();
		}
	}
}
