// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_ClickToMove.h"

#include "AbilitySystem/Abilities/AuraAbility_ClickToMove.h"

UAbilityTask_ClickToMove::UAbilityTask_ClickToMove()
{
	bTickingTask = true;
}

UAbilityTask_ClickToMove* UAbilityTask_ClickToMove::CreateTask(UAuraAbility_ClickToMove* OwningAbility)
{
	UAbilityTask_ClickToMove* Task = NewAbilityTask<UAbilityTask_ClickToMove>(OwningAbility);
	if (Task)
	{
		Task->OwningAbility = OwningAbility;
	}
	return Task;
}

void UAbilityTask_ClickToMove::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (OwningAbility && OwningAbility->MoveToDestination())
	{
		// 캐릭터가 목적지에 도착하면 Task 종료 및 델레게이트 실행
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnArrivedDelegate.Execute();
		}
		EndTask();
	}
}
