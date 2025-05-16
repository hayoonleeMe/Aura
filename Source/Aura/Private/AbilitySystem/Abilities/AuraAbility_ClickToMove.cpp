// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_ClickToMove.h"

#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_MoveAlongNavPath.h"
#include "Aura/Aura.h"
#include "GameFramework/PlayerController.h"
#include "Interface/InteractionInterface.h"
#include "Navigation/AvoidBeaconQueryFilter.h"

const FVector UAuraAbility_ClickToMove::ProjectBoxExtent(1000.f);

UAuraAbility_ClickToMove::UAuraAbility_ClickToMove()
{
	AbilityTags.AddTag(AuraGameplayTags::Abilities_ClickToMove);
	CancelAbilitiesWithTag.AddTag(AuraGameplayTags::Abilities_TryInteract);
	StartupInputTag = AuraGameplayTags::InputTag_RMB;
	
	bShouldMove = true;
	bShouldSpawnCursorEffect = true;
	InputDelay = 0.03f;
	bProcessInput = true;
	InitialArriveAcceptanceRadius = ArriveAcceptanceRadius;
}

void UAuraAbility_ClickToMove::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Pawn = CastChecked<APawn>(GetAvatarActorFromActorInfo());
	PlayerController = CastChecked<APlayerController>(Pawn->GetController());

	bShouldMove = true;
	bShouldSpawnCursorEffect = true;	// 처음 마우스 클릭하면 커서 효과 생성
	ArriveAcceptanceRadius = InitialArriveAcceptanceRadius;

	GetWorld()->GetTimerManager().SetTimer(DelayTimer, FTimerDelegate::CreateLambda([this]()
	{
		bProcessInput = true;
	}), InputDelay, true);
}

void UAuraAbility_ClickToMove::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo)
{
	// InputDelay마다 수행
	if (bProcessInput)
	{
		bProcessInput = false;
		
		FHitResult CursorHit;
		PlayerController->GetHitResultUnderCursor(ECC_ClickToMove, false, CursorHit);
		if (!CursorHit.bBlockingHit)
		{
			return;
		}

		const IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(CursorHit.GetActor());
		if (InteractionInterface && InteractionInterface->GetOverrideArriveAcceptanceRadius() > 0.f)
		{
			// ArriveAcceptanceRadius 업데이트
			ArriveAcceptanceRadius = InteractionInterface->GetOverrideArriveAcceptanceRadius();

			// Cursor 생성 방지
			bShouldSpawnCursorEffect = false;
		}
		else
		{
			ArriveAcceptanceRadius = InitialArriveAcceptanceRadius;
		}

		if (UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
		{
			FVector PathEnd = FVector::ZeroVector;
			TSubclassOf<UNavigationQueryFilter> QueryFilter = nullptr;

			if (InteractionInterface)
			{
				// Interactable 액터로 이동할 때, 그 액터의 NavArea는 이동 가능해야 한다.
				PathEnd = UAuraBlueprintLibrary::GetActorFeetLocation(CursorHit.GetActor());
			}
			else
			{
				FNavLocation ProjectedLocation;
				if (!NavSystem->ProjectPointToNavigation(CursorHit.ImpactPoint, ProjectedLocation, ProjectBoxExtent))
				{
					return;
				}
				PathEnd = ProjectedLocation;
				// Beacon을 피해가야함
				QueryFilter = UAvoidBeaconQueryFilter::StaticClass();
			}

			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(Pawn, Pawn->GetNavAgentLocation(), PathEnd, nullptr, QueryFilter))
			{
				PathIndex = 0;
				NavPaths = MoveTemp(NavPath->PathPoints);
				if (NavPaths.IsEmpty())
				{
					return;
				}
			}
		}

		// 장애물을 피한 NavPath를 따른 위치에 커서 효과 생성
		if (CursorEffect && bShouldSpawnCursorEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(Pawn, CursorEffect, NavPaths.Last());
			bShouldSpawnCursorEffect = false;
		}

		// 목적지까지 이동하는 AbilityTask 생성 및 실행
		if (!IsValid(AbilityTask_MoveAlongNavPath))
		{
			if (AbilityTask_MoveAlongNavPath = UAbilityTask_MoveAlongNavPath::CreateTask(this); AbilityTask_MoveAlongNavPath)
			{
				// MoveToDestination 함수 등록
				AbilityTask_MoveAlongNavPath->MoveToDestinationDelegate.BindUObject(this, &ThisClass::MoveToDestination);
				
				// 목적지에 도착하고 Key를 떼면 Ability 종료
				AbilityTask_MoveAlongNavPath->OnArrivedDelegate.BindLambda([this]()
				{
					if (!bShouldMove)
					{
						DelayTimer.Invalidate();
						K2_EndAbility();
					}
				});
				AbilityTask_MoveAlongNavPath->ReadyForActivation();
			}
		}	
	}
}

void UAuraAbility_ClickToMove::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	bShouldMove = false;
	bShouldSpawnCursorEffect = true;	// 캐릭터가 이동 중에 다시 마우스 클릭하면 커서 효과 생성
}
