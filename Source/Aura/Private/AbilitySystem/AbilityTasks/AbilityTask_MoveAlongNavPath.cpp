// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_MoveAlongNavPath.h"

UAbilityTask_MoveAlongNavPath::UAbilityTask_MoveAlongNavPath()
{
	bTickingTask = true;
}

UAbilityTask_MoveAlongNavPath* UAbilityTask_MoveAlongNavPath::CreateTask(UGameplayAbility* OwningAbility)
{
	return NewAbilityTask<UAbilityTask_MoveAlongNavPath>(OwningAbility);
}

void UAbilityTask_MoveAlongNavPath::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (MoveToDestinationDelegate.IsBound())
	{
		if (MoveToDestinationDelegate.Execute())
		{
			// 캐릭터가 목적지에 도착하면 Task 종료 및 델레게이트 실행
			if (ShouldBroadcastAbilityTaskDelegates())
			{
				OnArrivedDelegate.Execute();
			}
			EndTask();		
		}
	}
}
