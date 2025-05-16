// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_MoveAlongNavPath.generated.h"

class UAuraAbility_MoveAlongNavPathBase;

DECLARE_DELEGATE(FOnArrivedSignature);

/**
 * @brief 매 틱마다 호출되어, 실제 이동 로직의 한 단계를 수행하고 최종 목적지에 도달했는지 여부를 반환합니다.
 * 이 델리게이트에 바인딩된 함수는 다음을 수행해야 합니다:
 * 1. 필요한 경우 캐릭터의 이동을 업데이트하거나 다음 이동 지점으로의 이동을 처리합니다.
 * 2. 이동 태스크의 최종 목적지에 도달했는지 여부를 판단합니다.
 * 
 * @return 최종 목적지에 도달했으면 true를 반환하고, 그렇지 않으면 false를 반환합니다.
 * true를 반환하면 UAbilityTask_MoveAlongNavPath 태스크는 OnArrivedDelegate를 호출하고 종료됩니다.
 * false를 반환하면 태스크는 다음 틱에서 계속 이 델리게이트를 호출하여 이동을 시도합니다.
 */
DECLARE_DELEGATE_RetVal(bool, FMoveToDestinationSignature);

/**
 * @brief GameplayAbility 내에서 내비게이션 경로를 따라 액터를 이동시키는 태스크입니다.
 * 
 * @details 매 틱마다 MoveToDestinationDelegate를 호출하여 이동을 처리하고,
 * 목적지에 도착하면 OnArrivedDelegate를 브로드캐스트합니다.
 */
UCLASS()
class AURA_API UAbilityTask_MoveAlongNavPath : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAbilityTask_MoveAlongNavPath();
	static UAbilityTask_MoveAlongNavPath* CreateTask(UGameplayAbility* OwningAbility);
	virtual void TickTask(float DeltaTime) override;

	// 캐릭터가 목적지에 도착하면 실행되는 델레게이트
	FOnArrivedSignature OnArrivedDelegate;

	// 매 틱마다 이동 로직을 수행하고 목적지 도착 여부를 반환하는 델리게이트입니다.
	// 이 델리게이트는 외부(주로 이 태스크를 생성한 GameplayAbility)에서 바인딩해야 합니다.
	FMoveToDestinationSignature MoveToDestinationDelegate;
};