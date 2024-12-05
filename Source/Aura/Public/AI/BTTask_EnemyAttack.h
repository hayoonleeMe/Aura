// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_EnemyAttack.generated.h"

/**
 * BlackboardKey Object를 CombatTarget으로 Enemy AI의 Attack Ability를 실행하는 BTTask
 */
UCLASS()
class AURA_API UBTTask_EnemyAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_EnemyAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
