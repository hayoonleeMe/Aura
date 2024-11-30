// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_ClickToMove.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_ClickToMove.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Player/AuraPlayerController.h"

UAuraAbility_ClickToMove::UAuraAbility_ClickToMove()
{
	bShouldMove = true;
	SquaredArriveAcceptanceRadius = 1600.f;
	bShouldSpawnCursorEffect = true;
	InputDelay = 0.03f;
	bProcessInput = true;
}

void UAuraAbility_ClickToMove::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Character = CastChecked<ACharacter>(GetAvatarActorFromActorInfo());
	AuraPlayerController = CastChecked<AAuraPlayerController>(Character->GetController());
	SplineComponent = AuraPlayerController->GetSplineComponent();
	check(SplineComponent);

	bShouldMove = true;
	bShouldSpawnCursorEffect = true;	// 처음 마우스 클릭하면 커서 효과 생성

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
		AuraPlayerController->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
		if (!CursorHit.bBlockingHit)
		{
			return;
		}

		// 경로 계산
		if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(Character, Character->GetActorLocation(), CursorHit.ImpactPoint))
		{
			SplineComponent->SetSplineWorldPoints(NavPath->PathPoints);
			if (NavPath->PathPoints.Num())
			{
				Destination = NavPath->PathPoints.Last();
			}
		}

		// 장애물을 피한 NavPath를 따른 위치에 커서 효과 생성
		if (CursorEffect && bShouldSpawnCursorEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(Character, CursorEffect, Destination);
			bShouldSpawnCursorEffect = false;
		}

		// 목적지까지 이동하는 AbilityTask 생성 및 실행
		if (!IsValid(AbilityTask_ClickToMove))
		{
			AbilityTask_ClickToMove = UAbilityTask_ClickToMove::CreateTask(this);
			if (AbilityTask_ClickToMove)
			{
				// 목적지에 도착하고 Key를 떼면 Ability 종료
				AbilityTask_ClickToMove->OnArrivedDelegate.BindLambda([this]()
				{
					if (!bShouldMove)
					{
						DelayTimer.Invalidate();
						K2_EndAbility();
					}
				});
				AbilityTask_ClickToMove->ReadyForActivation();
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

bool UAuraAbility_ClickToMove::MoveToDestination() const
{
	// Destination까지 NavPath에 따라 설정된 Spline Points를 향해 캐릭터를 이동시킨다.
	const FVector LocationOnSpline = SplineComponent->FindLocationClosestToWorldLocation(Character->GetActorLocation(), ESplineCoordinateSpace::World);
	const FVector Direction = SplineComponent->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
	Character->AddMovementInput(Direction);

	// 목적지에 도착
	const float SquaredDistToDest = (LocationOnSpline - Destination).SquaredLength();
	if (SquaredDistToDest <= SquaredArriveAcceptanceRadius)
	{
		return true;
	}
	return false;
}
