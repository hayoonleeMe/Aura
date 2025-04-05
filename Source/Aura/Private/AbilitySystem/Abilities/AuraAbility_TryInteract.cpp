// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_TryInteract.h"

#include "AuraGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_ClickToMove.h"
#include "Interface/InteractionInterface.h"
#include "Player/AuraPlayerController.h"

UAuraAbility_TryInteract::UAuraAbility_TryInteract()
{
	AbilityTags.AddTag(AuraGameplayTags::Abilities_TryInteract);
	CancelAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_ClickToMove);
	StartupInputTag = AuraGameplayTags::InputTag_Interact;
	
	InitialArriveAcceptanceRadius = ArriveAcceptanceRadius;
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

	if (!IsValid(TargetActor) || !InteractionInterface)
	{
		K2_EndAbility();
		return;
	}

	// ArriveAcceptanceRadius 업데이트
	if (InteractionInterface->GetOverrideArriveAcceptanceRadius() > 0.f)
	{
		ArriveAcceptanceRadius = InteractionInterface->GetOverrideArriveAcceptanceRadius();
	}
	
	// 충분히 가까우면 바로 상호작용
	const float SquaredDistToDest = (Pawn->GetActorLocation() - TargetActor->GetActorLocation()).SquaredLength();
	if (SquaredDistToDest <= ArriveAcceptanceRadius * ArriveAcceptanceRadius)
	{
		Interact();
		return;
	}

	// 경로 계산
	if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(Pawn, Pawn->GetActorLocation(), TargetActor->GetActorLocation()))
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
	if (UAbilityTask_ClickToMove* Task = UAbilityTask_ClickToMove::CreateTask(this))
	{
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
