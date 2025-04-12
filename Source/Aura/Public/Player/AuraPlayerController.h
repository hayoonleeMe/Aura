// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interface/PlayerInterface.h"
#include "Types/GameMenuType.h"
#include "Types/StageStatus.h"
#include "AuraPlayerController.generated.h"

class UAttributeConfig;
class USpellConfig;
class ULevelSequenceManageComponent;
class UInputAction;
class UWidgetComponent;
class UDamageIndicatorComponent;
class USplineComponent;
class UAuraAbilitySystemComponent;
class UInputMappingContext;
class UAuraInputConfig;

// Stage Status가 변경됨을 알리는 델레게이트
DECLARE_DELEGATE_FourParams(FOnStageStatusChangedSignature, EStageStatus /*StageStatus*/, int32 /*StageNumber*/, double /*WaitingTimerEndSeconds*/, int32 /*TotalEnemyCount*/);

// 현재 스테이지의 전체 Enemy 수가 변경됨을 알리는 델레게이트
DECLARE_DELEGATE_OneParam(FOnTotalEnemyCountChangedSignature, int32 /*TotalEnemyCount*/);

// Respawn이 시작됨을 알리는 델레게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRespawnStartedSignature, float /*RespawnTimerEndSeconds*/);

// 게임이 종료됨을 알리는 델레게이트
DECLARE_DELEGATE(FOnGameEndSignature);

// 월드에 존재하던 적이 죽음을 알리는 델레게이트
DECLARE_DELEGATE(FOnEnemyDeadSignature);

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	virtual void OnRep_Pawn() override;

	/* Begin PlayerInterface */
	virtual FHitResult GetTargetHitResult() const override { return TargetHitResult; }
	virtual void IndicateAbilityActivateCostFail() override;
	virtual void IndicateAbilityActivateCooldownFail() override;
	virtual void NotifyEnemyDead() override;
	virtual void EnableUIInput() override;
	virtual void DisableUIInput() override;
	virtual void EnableCinematicInput() override;
	virtual void DisableCinematicInput() override;
	virtual FKey GetInteractKeyMappedToAction() const override;
	virtual UAuraInputConfig* GetAuraInputConfig() const override { return AuraInputConfig; }
	virtual USpellConfig* GetSpellConfig() const override { return SpellConfig; }
	virtual UAttributeConfig* GetAttributeConfig() const override { return AttributeConfig; }
	virtual FOnLevelSequenceStopSignature* GetOnLevelSequenceStopDelegate() const override;
	virtual void PlayLevelSequence(const FName& LevelSequenceTag) override;
	virtual void StopLevelSequence(const FName& LevelSequenceTag) override;
	virtual void SetLevelSequenceActorLocation(const FName& LevelSequenceTag, const FVector& NewLocation) override;
	virtual void HandleInitialLogic() const override;
	virtual void FlushPendingStageWaitingBroadcast() override;
	/* End PlayerInterface */

	// PreStageHUD Widget을 표시한다.
	void ShowPreStageHUD() const;

	// 모든 메뉴에 대한 MenuShortcutAlert Widget을 표시한다.
	void ShowAllMenuShortcutAlert() const;

	void EnableAbilityInput();
	void DisableAbilityInput();

	AActor* GetTargetActorFromCursor() const { return TargetFromCurrentFrame.Get(); }

	// Damage를 나타내는 DamageIndicator Widget을 화면에 표시한다.
	UFUNCTION(Client, Reliable)
	void ClientIndicateDamage(float Damage, bool bIsBlockedHit, bool bIsCriticalHit, const FVector_NetQuantize& TargetLocation) const;

	FORCEINLINE int32 GetUsedLifeCount() const { return UsedLifeCount; }
	FORCEINLINE void UseLifeCount() { ++UsedLifeCount; }

	// ============================================================================
	// Stage
	// ============================================================================

	// AStageGameMode에 플레이어의 ASC가 초기화됐음을 알린다.
	UFUNCTION(Server, Reliable)
	void ServerNotifyASCInitToGameMode();

	UFUNCTION(Client, Reliable)
	void ClientOnStageStatusChanged(EStageStatus StageStatus, int32 StageNumber, double WaitingTimerEndSeconds, int32 TotalEnemyCount);

	UFUNCTION(Client, Reliable)
	void ClientOnTotalEnemyCountChanged(int32 TotalEnemyCount);

	FOnStageStatusChangedSignature OnStageStatusChangedDelegate;
	FOnTotalEnemyCountChangedSignature OnTotalEnemyCountChangedDelegate;
	FOnEnemyDeadSignature OnEnemyDeadDelegate;

	// ============================================================================
	// Respawn
	// ============================================================================

	UFUNCTION(Client, Reliable)
	void ClientNotifyRespawnStart(float RespawnTimerEndSeconds);

	FOnRespawnStartedSignature OnRespawnStartedDelegate;

	// ============================================================================
	// Game End
	// ============================================================================

	UFUNCTION(Client, Reliable)
	void ClientEndGame();

	FOnGameEndSignature OnGameEndDelegate;
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	
private:
	// ============================================================================
	// Combat
	// ============================================================================

	void CursorTrace();

	// CursorTrace 활성화/비활성화
	void EnableCursorTrace(bool bEnabled);
	
	// CursorTrace()를 호출할 수 있는지 여부
	bool bCursorTraceEnabled = true;

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

	void AbilityInputPressed(int32 InputID);
	void AbilityInputReleased(int32 InputID);

	// Input Event와 Ability 연동
	void BindAbilityInput();

	// 현재 적용된 Input Mapping Context 상태를 저장하는 플래그
	uint8 IMCFlags = 0;

	void StoreInputMappingContextState();
	void RestoreInputMappingContextState();

	// Ability Input을 비활성화한 횟수
	// 0일 때만 Ability Input Mapping Context를 추가할 수 있다.
	int32 AbilityInputBlockCount = 0;

	// Cinematic Input을 활성화한 횟수
	// 0일 때만 Cinematic Input Mapping Context를 제거할 수 있다.
	int32 CinematicInputAddCount = 0;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<UInputMappingContext> AbilityContext;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<UInputMappingContext> CommonContext;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<UInputMappingContext> UIContext;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<UInputMappingContext> CinematicContext;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<UInputAction> IA_AttributeMenu;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<UInputAction> IA_SpellMenu;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<UInputAction> IA_PauseMenu;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<UInputAction> IA_TutorialMenu;

	// Menu Input Action이 Started 될 때 호출되는 콜백 함수
	void OnMenuActionStarted(EGameMenuType GameMenuType);
	
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<UInputAction> IA_CloseUI;

	// CloseUI Input Action이 Started 될 때 호출되는 콜백 함수
	void OnCloseUIActionStarted();

	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<UInputAction> IA_CloseCinematic;

	// CloseCinematic Input Action이 Started 될 때 호출되는 콜백 함수
	void OnCloseCinematicActionStarted();

	// ============================================================================
	// Data
	// ============================================================================

	UPROPERTY(EditDefaultsOnly, Category="Aura|Data")
	TObjectPtr<UAuraInputConfig> AuraInputConfig;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura|Data")
	TObjectPtr<USpellConfig> SpellConfig;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Data")
	TObjectPtr<UAttributeConfig> AttributeConfig;

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

	// ============================================================================
	// Level Sequence
	// ============================================================================

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ULevelSequenceManageComponent> LevelSequenceManageComponent;

	// PauseMenu Level Sequence Actor를 PlayerController의 Pawn에 부착한다.
	void AttachPauseMenuLevelSequenceActorToPawn() const;

	// ============================================================================
	// Stage
	// ============================================================================

	struct StageWaitingBroadcastParam
	{
		int32 StageNumber;
		double WaitingTimerEndSeconds;
		int32 TotalEnemyCount;
	};

	// StartStageBeacon이 생성되고 SpawnBeacon Level Sequence가 종료된 후, Waiting Stage Status에 대한 OnStageStatusChangedDelegate를 호출한다.
	// 클라이언트에서 StartStageBeacon이 아직 Replicate되지 않은 경우에는 Broadcast를 처리할 수 없으므로, 필요한 파라미터를 임시로 저장해 둔다.
	// StartStageBeacon의 생성과 Level Sequence 완료 후, 저장된 파라미터들을 Flush하여 Broadcast를 수행한다.
	TArray<StageWaitingBroadcastParam> PendingStageWaitingBroadcastParams;
};
