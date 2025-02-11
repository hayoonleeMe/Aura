// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_MoveAlongNavPathBase.h"

UAuraAbility_MoveAlongNavPathBase::UAuraAbility_MoveAlongNavPathBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	ArriveAcceptanceRadius = 25.f;
}

bool UAuraAbility_MoveAlongNavPathBase::MoveToDestination()
{
	// NavPaths가 유효하지 않으면 Ability Task가 생성되지 않아 해당 함수도 호출되지 않을 것으로 예상하지만,
	// 만약을 대비해 Ability Task를 종료하도록 true를 반환
	if (NavPaths.IsEmpty())
	{
		return true;
	}

	// 캐릭터의 바닥 지점 좌표
	const FVector PawnLocation = Pawn->GetNavAgentLocation();

	// 캐릭터가 향해야 할 NavPaths 상의 지점을 찾음
	for (; PathIndex < NavPaths.Num(); ++PathIndex)
	{
		const float SquaredDist = FVector::DistSquared(PawnLocation, NavPaths[PathIndex]);
		if (SquaredDist > ArriveAcceptanceRadius * ArriveAcceptanceRadius)
		{
			break;
		}
	}
	// 찾은 지점을 향해 캐릭터를 이동
	if (NavPaths.IsValidIndex(PathIndex))
	{
		const FVector Direction = (NavPaths[PathIndex] - PawnLocation).GetSafeNormal();
		Pawn->AddMovementInput(Direction);
	}

	// 목적지에 도착하면 Ability Task를 종료하도록 true 반환
	const float SquaredDistToDest = (PawnLocation - NavPaths.Last()).SquaredLength();
	if (SquaredDistToDest <= ArriveAcceptanceRadius * ArriveAcceptanceRadius)
	{
		return true;
	}

	// 계속 이동하기 위해 false 반환
	return false;
}
