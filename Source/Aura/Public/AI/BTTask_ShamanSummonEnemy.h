// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ShamanSummonEnemy.generated.h"

/**
 * ShamanSummonEnemy Ability를 실행하는 BTTask
 * 어빌리티가 종료되어야 Succeeded를 반환하는 Latent Action
 */
UCLASS()
class AURA_API UBTTask_ShamanSummonEnemy : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ShamanSummonEnemy();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetActorSelector;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector HasSummonedEnemySelector;
};
