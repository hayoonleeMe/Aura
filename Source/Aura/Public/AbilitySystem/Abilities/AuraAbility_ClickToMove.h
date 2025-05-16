// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraAbility_MoveAlongNavPathBase.h"
#include "AuraAbility_ClickToMove.generated.h"

class UNiagaraSystem;
class UAbilityTask_MoveAlongNavPath;

/**
 * 클릭한 마우스 커서 위치로 Navigation System에 의한 Path를 따라 캐릭터를 이동시키는 Ability 
 */
UCLASS(HideCategories=("Aura|Spell"))
class AURA_API UAuraAbility_ClickToMove : public UAuraAbility_MoveAlongNavPathBase
{
	GENERATED_BODY()

public:
	UAuraAbility_ClickToMove();
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
private:
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_MoveAlongNavPath> AbilityTask_MoveAlongNavPath;

	// 캐릭터가 계속 목적지로 이동해야 하는지를 나타내는 변수
	uint8 bShouldMove : 1;
	
	// InputPressed()의 경로 설정 로직을 InputDelay마다 수행하도록 구현  
	FTimerHandle DelayTimer;
	float InputDelay;
	uint8 bProcessInput : 1;

	// 마우스 커서 위치를 NavMesh 상의 위치로 보정하기 위해 사용되는 BoxExtent
	static const FVector ProjectBoxExtent;

	// ArriveAcceptanceRadius의 기본값 캐싱 
    float InitialArriveAcceptanceRadius = 0.f;

	// ============================================================================
	// Effect
	// ============================================================================

	UPROPERTY(EditDefaultsOnly, Category="Aura|Effect")
	TObjectPtr<UNiagaraSystem> CursorEffect;

	// true일 때 커서 효과 생성
	uint8 bShouldSpawnCursorEffect : 1;
};
