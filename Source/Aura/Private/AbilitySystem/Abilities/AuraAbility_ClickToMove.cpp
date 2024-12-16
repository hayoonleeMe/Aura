// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_ClickToMove.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_ClickToMove.h"
#include "GameFramework/Character.h"

UAuraAbility_ClickToMove::UAuraAbility_ClickToMove()
{
	bShouldMove = true;
	SquaredArriveAcceptanceRadius = 600.f;
	bShouldSpawnCursorEffect = true;
	InputDelay = 0.03f;
	bProcessInput = true;
	PathIndex = 0;
}

void UAuraAbility_ClickToMove::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Character = CastChecked<ACharacter>(GetAvatarActorFromActorInfo());
	PlayerController = CastChecked<APlayerController>(Character->GetController());

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
		PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
		if (!CursorHit.bBlockingHit)
		{
			return;
		}

		// 경로 계산
		if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(Character, Character->GetActorLocation(), CursorHit.ImpactPoint))
		{
			PathIndex = 0;
			NavPaths = MoveTemp(NavPath->PathPoints);
			if (NavPaths.IsEmpty())
			{
				return;
			}
		}

		// 장애물을 피한 NavPath를 따른 위치에 커서 효과 생성
		if (CursorEffect && bShouldSpawnCursorEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(Character, CursorEffect, NavPaths.Last());
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

bool UAuraAbility_ClickToMove::MoveToDestination()
{
	// NavPaths가 유효하지 않으면 Ability Task가 생성되지 않아 해당 함수도 호출되지 않을 것으로 예상하지만,
	// 만약을 대비해 Ability Task를 종료하도록 true를 반환
	if (NavPaths.IsEmpty())
	{
		return true;
	}

	// 캐릭터의 바닥 지점 좌표
	const FVector CharacterLocation = Character->GetNavAgentLocation();

	// 캐릭터가 향해야 할 NavPaths 상의 지점을 찾음
	for (; PathIndex < NavPaths.Num(); ++PathIndex)
	{
		const float SquaredDist = FVector::DistSquared(CharacterLocation, NavPaths[PathIndex]);
		if (SquaredDist > SquaredArriveAcceptanceRadius)
		{
			break;
		}
	}
	// 찾은 지점을 향해 캐릭터를 이동
	if (NavPaths.IsValidIndex(PathIndex))
	{
		const FVector Direction = (NavPaths[PathIndex] - CharacterLocation).GetSafeNormal();
		Character->AddMovementInput(Direction);
	}

	// 목적지에 도착하면 Ability Task를 종료하도록 true 반환
	const float SquaredDistToDest = (CharacterLocation - NavPaths.Last()).SquaredLength();
	if (SquaredDistToDest <= SquaredArriveAcceptanceRadius)
	{
		return true;
	}

	// 계속 이동하기 위해 false 반환
	return false;
}
