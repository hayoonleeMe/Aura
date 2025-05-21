// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageAbility.h"
#include "AuraAbility_CastConfirmBase.generated.h"

class ARangeDecalActor;

/**
 * 처음 실행하면 마우스 커서 위치에 스펠의 범위를 표시하고 원하는 위치에서 LMB를 입력하면 실제 기능을 수행하는 Abstract Base Class이다.
 * 처음 실행하고 다시 어빌리티 키를 입력하거나 다른 어빌리티를 실행하면 취소된다.
 */
UCLASS(Abstract)
class AURA_API UAuraAbility_CastConfirmBase : public UAuraDamageAbility
{
	GENERATED_BODY()

public:
	UAuraAbility_CastConfirmBase();
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
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

	// RangeDecalActor로 표시할 스펠 범위를 반환한다.
	// 서브 클래스에서 원하는 범위를 반환하도록 구현해야 한다.
	virtual float GetRangeRadius() const { return 0.f; }

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
	virtual void OnTargetDataUnderMouseSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle) {}

	// 캐릭터 이동을 멈추도록 이동 관련 어빌리티를 취소한다.
	void BlockMoveAbilities() const;  

	UPROPERTY(EditDefaultsOnly, Category="Aura|CastConfirmBase")
	TSubclassOf<ARangeDecalActor> RangeDecalActorClass;

	UPROPERTY()
	TObjectPtr<ARangeDecalActor> RangeDecalActor;

	// 벽을 제외한 맵의 범위를 나타내는 Box
    FBox PlayableBounds;
    
    // Spell의 최대 시전 사거리
    UPROPERTY(EditDefaultsOnly, Category="Aura|CastConfirmBase")
    float MaxCastRange;
};
