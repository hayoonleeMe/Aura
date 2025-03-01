// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "Interface/PlayerInterface.h"
#include "Types/StageStatus.h"
#include "AuraPlayerController.generated.h"

class UWidgetComponent;
class UDamageIndicatorComponent;
class USplineComponent;
class UAuraAbilitySystemComponent;
class UInputMappingContext;
class UAuraInputConfig;

// 클라이언트에서 GameStateBase가 유효해질 때 Broadcast
DECLARE_MULTICAST_DELEGATE(FOnGameStateBaseValidInClientSignature);


// Respawn이 시작됨을 알리는 델레게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRespawnStartedSignature, float /*RespawnTimerEndSeconds*/);

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController, public IPlayerInterface
{
	GENERATED_BODY()

public:
	virtual void PlayerTick(float DeltaTime) override;
	virtual void OnRep_Pawn() override;

	/* Begin PlayerInterface */
	virtual FHitResult GetTargetHitResult() const override { return TargetHitResult; }
	virtual void IndicateAbilityActivateCostFail() override;
	virtual void IndicateAbilityActivateCooldownFail() override;
	virtual void NotifyEnemyDead() override;
	/* End PlayerInterface */

	AActor* GetTargetActorFromCursor() const { return TargetFromCurrentFrame.Get(); }
	
	// 게임에서 기본으로 사용하는 Input Mode로 설정
	void SetInGameInputMode();
	
	// UI 전용 Input Mode로 설정
	void SetUIInputMode(UUserWidget* WidgetToFocus);

	// Damage를 나타내는 DamageIndicator Widget을 화면에 표시한다.
	UFUNCTION(Client, Reliable)
	void ClientIndicateDamage(float Damage, bool bIsBlockedHit, bool bIsCriticalHit, const FVector_NetQuantize& TargetLocation) const;

	UFUNCTION(Client, Reliable)
	void ClientOnStageStatusChanged(EStageStatus StageStatus, int32 StageNumber, double WaitingTimerEndSeconds, int32 TotalEnemyCount);
	UFUNCTION(Client, Reliable)
	void ClientEndGame();

	bool IsValidGameStateBaseInClient() const { return bValidGameStateBaseInClient; }
	FOnGameStateBaseValidInClientSignature OnGameStateBaseValidInClientDelegate;

	FORCEINLINE int32 GetUsedLifeCount() const { return UsedLifeCount; }
	FORCEINLINE void UseLifeCount() { ++UsedLifeCount; }

	// AAuraGameStateBase에서 캐싱한 Level Sequence Player가 종료될 때 호출되는 콜백 함수
	// ViewTarget을 플레이어 캐릭터로 되돌린다.
	UFUNCTION()
	void OnLevelSequencePlayerStop();

	// ============================================================================
	// Respawn
	// ============================================================================

	UFUNCTION(Client, Reliable)
	void ClientNotifyRespawnStart(float RespawnTimerEndSeconds);

	FOnRespawnStartedSignature OnRespawnStartedDelegate;

	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	// AAuraGameStateBase에 캐싱된 PauseMenu Level Sequence Actor를 PlayerController의 Pawn에 부착한다.
	void AttachPauseMenuLevelSequenceActorToPawn() const;
	
private:
	// ============================================================================
	// Combat
	// ============================================================================

	void CursorTrace();

	// Cached Target HitResult Under Cursor
	FHitResult TargetHitResult;

	// 이전 프레임에서의 TargetHitResult의 Actor
	TWeakObjectPtr<AActor> TargetFromPrevFrame;

	// 현재 프레임에서의 TargetHitResult의 Actor
	TWeakObjectPtr<AActor> TargetFromCurrentFrame;

	// 플레이어가 사용한 목숨 개수
	int32 UsedLifeCount = 0;
	
	// ============================================================================
	// Input
	// ============================================================================

	void AbilityInputPressed(FGameplayTag InputTag, int32 InputID);
	void AbilityInputReleased(FGameplayTag InputTag, int32 InputID);

	// Input Event와 Ability 연동
	void BindAbilityInput();

	// 클라이언트에서 BindAbilityInput()를 수행하기 위해 Polling
	void PollInit();
	bool bValidGameStateBaseInClient = false;
	FTimerHandle PollingTimerHandle;

	// ============================================================================
	// GAS
	// ============================================================================

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();

	// ============================================================================
	// Damage Indicator
	// ============================================================================

	// Damage Indicator를 표시하는 데 사용할 WidgetComponent Class
	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage Indicator")
	TSubclassOf<UDamageIndicatorComponent> DamageIndicatorComponentClass;

	// ============================================================================
	// Cast Fail Indicator
	// ============================================================================

	// Damage Indicator를 표시하는 데 사용할 WidgetComponent Class
	UPROPERTY(EditDefaultsOnly, Category="Aura|Cast Fail Indicator")
	TSubclassOf<UWidgetComponent> CostCastFailIndicatorComponentClass;

	UPROPERTY()
	TObjectPtr<UWidgetComponent> CostCastFailIndicatorComponent;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Cast Fail Indicator")
	TSubclassOf<UWidgetComponent> CooldownCastFailIndicatorComponentClass;

	UPROPERTY()
	TObjectPtr<UWidgetComponent> CooldownCastFailIndicatorComponent;
};
