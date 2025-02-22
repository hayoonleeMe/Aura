// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Types/StageStatus.h"
#include "AuraHUD.generated.h"

class USpellMenuWidgetController;
class UAttributeMenuWidgetController;
struct FWidgetControllerParams;
class UOverlayWidgetController;
class UAuraUserWidget;
class UAttributeSet;
class UAbilitySystemComponent;

/**
 * Widget을 화면에 표시하고 WidgetController를 관리하는 HUD Class
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()

public:
	// Overlay Widget, WidgetController를 초기화하고 Overlay Widget을 화면에 표시
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

	UOverlayWidgetController* GetOverlayWidgetController() const { return OverlayWidgetController; }
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController() const { return AttributeMenuWidgetController; }
	USpellMenuWidgetController* GetSpellMenuWidgetController() const { return SpellMenuWidgetController; }

	// 현재 변경된 StageStatus 전달
	void OnStageStatusChanged(EStageStatus StageStatus, int32 StageNumber, double WaitingTimerEndSeconds, int32 TotalEnemyCount) const;

	// Respawn 시작을 알림
	void OnRespawnStart(double RespawnTimerEndSeconds) const;

	// Enemy 죽음을 전달
	void OnEnemyDead() const;

	// 게임 종료를 전달
	void OnGameEnd() const;

private:
	// ============================================================================
	// Overlay
	// ============================================================================

	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Overlay")
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Overlay")
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	// OverlayWidgetController가 유효하지 않다면 FWidgetControllerParams로 OverlayWidgetController를 생성
	void MakeOverlayWidgetController(const FWidgetControllerParams& WCParams);

	// ============================================================================
	// Attribute Menu
	// ============================================================================

	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Attribute Menu")
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;

	// AttributeMenuWidgetController가 유효하지 않다면 FWidgetControllerParams로 AttributeMenuWidgetController를 생성
	void MakeAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);

	// ============================================================================
	// Spell Menu
	// ============================================================================

	UPROPERTY()
	TObjectPtr<USpellMenuWidgetController> SpellMenuWidgetController;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura|Spell Menu")
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;

	// SpellMenuWidgetController가 유효하지 않다면 FWidgetControllerParams로 SpellMenuWidgetController를 생성
	void MakeSpellMenuWidgetController(const FWidgetControllerParams& WCParams);
};
