// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraAbility_MoveAlongNavPathBase.generated.h"

/**
 * NavPath를 따라 캐릭터를 이동시키는 기능을 구현하는 Abstract Ability Class
 */
UCLASS(Abstract)
class AURA_API UAuraAbility_MoveAlongNavPathBase : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UAuraAbility_MoveAlongNavPathBase();
	
	// NavPaths를 따라 캐릭터를 이동시키는 함수
	// 목적지에 도착하면 true를 반환하고, 계속 이동해야하면 false 반환
	// UAbilityTask_ClickToMove에 의해 호출됨
	bool MoveToDestination();

protected:
	// 이동할 Pawn
	UPROPERTY()
	TObjectPtr<APawn> Pawn;

	// UNavigationSystemV1에 의해 구해진 경로를 캐싱
	TArray<FVector> NavPaths;

	// NavPaths Point 추적
	int32 PathIndex = 0;
	
	// 목적지 도착 인정 범위
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	float ArriveAcceptanceRadius;
};
