// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_ClickToMove.generated.h"

DECLARE_DELEGATE(FOnArrived);

class UAuraAbility_ClickToMove;
/**
 * 
 */
UCLASS()
class AURA_API UAbilityTask_ClickToMove : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAbilityTask_ClickToMove();
	static UAbilityTask_ClickToMove* CreateTask(UAuraAbility_ClickToMove* OwningAbility);
	virtual void TickTask(float DeltaTime) override;

	// 캐릭터가 목적지에 도착하면 실행되는 델레게이트
	FOnArrived OnArrived;		
	
private:
	UPROPERTY()
	TObjectPtr<UAuraAbility_ClickToMove> OwningAbility;
};