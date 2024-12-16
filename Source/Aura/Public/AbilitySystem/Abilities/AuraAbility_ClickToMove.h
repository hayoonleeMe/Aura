// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraAbility_ClickToMove.generated.h"

class UNiagaraSystem;
class UAbilityTask_ClickToMove;
class AAuraPlayerController;
class USplineComponent;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbility_ClickToMove : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UAuraAbility_ClickToMove();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	bool MoveToDestination() const;
	
private:
	UPROPERTY()
	TObjectPtr<ACharacter> Character;
	
	UPROPERTY()
	TObjectPtr<AAuraPlayerController> AuraPlayerController;
	
	UPROPERTY()
	TObjectPtr<USplineComponent> SplineComponent;

	UPROPERTY()
	TObjectPtr<UAbilityTask_ClickToMove> AbilityTask_ClickToMove;

	FVector Destination;

	// 캐릭터가 계속 목적지로 이동해야 하는지를 나타내는 변수
	uint8 bShouldMove : 1;
	
	// 목적지 도착 인정 범위 (제곱값)
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	float SquaredArriveAcceptanceRadius;

	// InputPressed()의 경로 설정 로직을 InputDelay마다 수행하도록 구현  
	FTimerHandle DelayTimer;
	float InputDelay;
	uint8 bProcessInput : 1;

	// ============================================================================
	// Effect
	// ============================================================================

	UPROPERTY(EditDefaultsOnly, Category="Aura|Effect")
	TObjectPtr<UNiagaraSystem> CursorEffect;

	// true일 때 커서 효과 생성
	uint8 bShouldSpawnCursorEffect : 1;
};
