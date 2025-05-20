// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageAbility.h"
#include "AuraAbility_Firenado.generated.h"

class ARangeDecalActor;

/**
 * Fire Tornado를 소환해 일정 범위 내에 데미지를 입히는 Firenado Spell
 * 일정 범위 내에 일정 시간마다 데미지를 입히고 마지막에 큰 데미지를 입힌다.
 * 처음 실행하면 스펠의 범위를 표시하고 원하는 위치에서 LMB를 입력하면 실제 데미지를 입히는 Firenado를 소환한다.
 * 처음 실행하고 다시 어빌리티 키를 입력하거나 다른 어빌리티를 실행하면 취소된다.
 */
UCLASS()
class AURA_API UAuraAbility_Firenado : public UAuraDamageAbility
{
	GENERATED_BODY()

public:
	UAuraAbility_Firenado();
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	virtual FText GetDescription(int32 Level) const override;

	// MaxCastRange를 적용한 TargetLocation을 계산하고 반환한다.
	FVector ComputeValidTargetLocation(const FVector& TargetLocation) const; 
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	// 어빌리티에 대한 InputPressed()가 이미 호출됐는지 여부
	// 입력이 들어오면 어빌리티를 Activate하고 이어서 InputPressed()가 호출되므로 두번째 입력이 들어옴을 알기 위해 사용한다. 
	bool bAlreadyPressed = false;

	// bShow가 true면 ARangeDecalActor를 화면에 표시하고(없으면 생성), false면 숨긴다. 
	void ShowRangeDecalActor(bool bShow);

	/**
	 * ASC의 GenericLocalConfirmCallbacks 델레게이트에 등록된 콜백 함수
	 * 특정 InputAction에 의한 입력이 들어오면 위 델레게이트를 브로드캐스트한다.
	 * @see AAuraPlayerController::OnConfirmSpellInputActionStarted()
	 */
	UFUNCTION()
	void OnLocalConfirmed();

	// 처음 실행 후 다음 입력을 기다리는 상태가 종료될 때 로직을 처리한다.
	// LMB 키를 입력하거나 어빌리티 키를 추가 입력하거나 다른 어빌리티 실행으로 취소될 때 호출된다.
	void HandleConfirmation();

	// UAbilityTask_TargetDataUnderMouse를 실행한다.
	void SetOrWaitTargetData();
	
	// UAbilityTask_TargetDataUnderMouse::TargetDataUnderMouseSetDelegate 델레게이트의 Callback 함수
	void OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	virtual void OnEventReceived(FGameplayEventData Payload) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Aura|Firenado")
	TSubclassOf<ARangeDecalActor> RangeDecalActorClass;

	UPROPERTY()
	TObjectPtr<ARangeDecalActor> RangeDecalActor;

	// 실제 Firenado를 소환할 위치
	// OnTargetDataUnderMouseSet()에서 저장되고, ComputeValidTargetLocation()를 통해 실제 유효한 위치로 보정해 사용한다.
	FVector CachedTargetLocation;

	// AFirenado Actor를 생성한다.
	void SpawnFirenado();
	
	// Spell의 최대 시전 사거리
	UPROPERTY(EditDefaultsOnly, Category="Aura|Firenado")
	float MaxCastRange;

	// 데미지를 입히는 범위
	UPROPERTY(EditDefaultsOnly, Category="Aura|Firenado")
	float EffectiveRadius;

	// 마지막 폭발 데미지를 입히는 범위
	UPROPERTY(EditDefaultsOnly, Category="Aura|Firenado")
	float ExplosionEffectiveRadius;

	// 데미지를 입히는 범위에 곱해질 Rate
	UPROPERTY(EditDefaultsOnly, Category="Aura|Firenado")
	FScalableFloat ScaleRateCurve;

	// Level에 따른 ScaleRate 값 반환
	float GetScaleRateByLevel(float Level) const;

	// DamageInterval마다 가해질 데미지는 UAuraDamageAbility::DamageCurve를 사용한다.
	// MakeDamageEffectParams()에서 자동으로 설정된다.

	// 마지막 데미지를 결정하기 위해 DamageInterval마다 가해지는 데미지에 곱해질 Rate
	UPROPERTY(EditDefaultsOnly, Category="Aura|Firenado")
	FScalableFloat FinalDamageRateCurve;

	// Level에 따른 ScaleRate 값 반환
	float GetFinalDamageRateByLevel(float Level) const;

	// 데미지를 가하는 시간 간격 (초)
	UPROPERTY(EditDefaultsOnly, Category="Aura|Firenado")
	float DamageInterval;

	// 데미지를 가하는 총 횟수
	UPROPERTY(EditDefaultsOnly, Category="Aura|Firenado")
	int32 TotalDamageCount;
};
