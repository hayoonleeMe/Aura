// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetController.h"
#include "SpellMenuWidgetController.h"
#include "Types/StageStatus.h"
#include "OverlayWidgetController.generated.h"

class UMultiplayerSessionsSubsystem;

/**
 * WBP_Overlay에서 생성해 연 Menu를 나타내는 enum
 */
UENUM(BlueprintType)
enum class EMenuType : uint8
{
	SpellMenu,
	AttributeMenu,
	PauseMenu
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

// WBP_Overlay에서 생성해 연 Menu Widget이 닫혔음을 알리는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMenuClosedSignature, EMenuType, MenuType);

// InputTag에 등록된 Spell의 Cooldown이 시작됨을 알리는 델레게이트
// Spell의 Cooldown을 전달
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquippedSpellCooldownStartSignature, const FGameplayTag&, InputTag, float, Cooldown);

// InputTag에 등록된 Spell의 Cooldown이 끝남을 알리는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedSpellCooldownEndSignature, const FGameplayTag&, InputTag);

// 현재 Stage Status가 변경됨을 알리는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnStageStatusChangedSignature, EStageStatus, StageStatus, int32, StageNumber, double, WaitingTimerEndSeconds, int32, TotalEnemyCount);

// Respawn 시작을 알리는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRespawnStartSignature, double, RespawnTimerEndSeconds);

// Enemy 죽음을 알리는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDeadSignature);

// 게임 종료를 알리는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameEndSignature);

/**
 * Overlay Widget에서 사용할 WidgetController
 */
UCLASS()
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnAttributeChangedSignature OnHealthChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnAttributeChangedSignature OnMaxHealthChangedDelegate;
	
	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnAttributeChangedSignature OnManaChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnAttributeChangedSignature OnMaxManaChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnAttributeChangedSignature OnXPChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnAttributeChangedSignature OnLevelChangedDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Aura|Overlay")
	FOnMenuClosedSignature OnMenuClosedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnEquippedSpellChangedSignature OnEquippedSpellChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnEquippedSpellCooldownStartSignature OnEquippedSpellCooldownStartDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnEquippedSpellCooldownEndSignature OnEquippedSpellCooldownEndDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnStageStatusChangedSignature OnStageStatusChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnRespawnStartSignature OnRespawnStartDelegate;
	
	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnEnemyDeadSignature OnEnemyDeadDelegate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Overlay")
	FOnGameEndSignature OnGameEndDelegate;

	// 현재 변경된 StageStatus 전달
	void OnStageStatusChanged(EStageStatus StageStatus, int32 StageNumber, double WaitingTimerEndSeconds, int32 TotalEnemyCount) const;

	// Respawn 시작을 알림
	void OnRespawnStart(double RespawnTimerEndSeconds) const;

	UFUNCTION(BlueprintCallable)
	int32 GetTotalLifeCount() const;

	// Enemy 죽음을 전달
	void OnEnemyDead() const;

	// 게임 종료를 전달
	void OnGameEnd() const;

	// 현재 게임에서 나가고 메인 메뉴로 돌아감
	UFUNCTION(BlueprintCallable)
	void LeaveGame();

private:
	// Spell의 장착 상태 변경을 업데이트
	void UpdateEquippedSpellChange(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag) const;

	// Startup Spell들을 업데이트
	void UpdateStartupSpells() const;

	// Equipped Spell의 Cooldown을 업데이트
	void UpdateEquippedSpellCooldown(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag) const;

	UPROPERTY()
	TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;

	// Destroy Session 작업이 완료됐을 때의 콜백 함수
	void OnDestroySessionComplete(bool bWasSuccessful) const;
};
