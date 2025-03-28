// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraUserWidget.h"
#include "Types/StageStatus.h"
#include "GameOverlay.generated.h"

class UGameEndAlert;
class URespawnTimer;
struct FGameplayTag;
class UStageInfoHUD;
class UStageWaitingTimer;
class UStageStartAlert;
class UPauseMenu;
class USpellMenu;
class UAttributeMenu;
class ULifeCounter;
class USquareButton;
class UHealthManaSpells;

/**
 * 
 */
UCLASS()
class AURA_API UGameOverlay : public UAuraUserWidget
{
	GENERATED_BODY()

public:
	UGameOverlay(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHealthManaSpells> HealthManaSpells;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<ULifeCounter> LifeCounter;

	// ============================================================================
	// Respawn Timer
	// ============================================================================

	UPROPERTY(EditDefaultsOnly, Category="Respawn Timer")
	TSubclassOf<URespawnTimer> RespawnTimerClass;

	void OnRespawnStarted(float RespawnTimerEndSeconds);

	// ============================================================================
	// Game End Alert
	// ============================================================================

	UPROPERTY(EditDefaultsOnly, Category="Game End Alert")
	TSubclassOf<UGameEndAlert> GameEndAlertClass;
	
	void OnGameEnd();

	// ============================================================================
	// Attribute Menu
	// ============================================================================

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_AttributeMenu;

	UFUNCTION()
	void OpenAttributeMenu();

	UPROPERTY(EditDefaultsOnly, Category="Attribute Menu")
	TSubclassOf<UAttributeMenu> AttributeMenuClass;
	
	UPROPERTY()
	TObjectPtr<UAttributeMenu> AttributeMenu;

	void OnAttributeMenuClosed();

	void OnAttributePointsChanged(int32 Value) const;

	// ============================================================================
	// Spell Menu
	// ============================================================================

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_SpellMenu;

	UFUNCTION()
	void OpenSpellMenu();

	UPROPERTY(EditDefaultsOnly, Category="Spell Menu")
	TSubclassOf<USpellMenu> SpellMenuClass;

	UPROPERTY()
	TObjectPtr<USpellMenu> SpellMenu;

	void OnSpellMenuClosed();

	void OnSpellPointsChanged(int32 Value) const;

	void OnSpellChanged(const FGameplayTag& SpellTag);
	void OnEquippedSpellChanged(bool bEquipped, const FGameplayTag& InputTag, const FGameplayTag& SpellTag);

	// ============================================================================
	// Pause Menu
	// ============================================================================

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_PauseMenu;

	UFUNCTION()
	void OpenPauseMenu();
	
	UPROPERTY(EditDefaultsOnly, Category="Pause Menu")
	TSubclassOf<UPauseMenu> PauseMenuClass;

	UPROPERTY()
	TObjectPtr<UPauseMenu> PauseMenu;

	void OnPauseMenuClosed();

	// ============================================================================
	// Stage
	// ============================================================================

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UNamedSlot> NamedSlot_StageWaiting;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UNamedSlot> NamedSlot_StageStartAlert;

	void OnStageStatusChanged(EStageStatus StageStatus, int32 StageNumber, double WaitingTimerEndSeconds, int32 TotalEnemyCount);
	void OnTotalEnemyCountChanged(int32 TotalEnemyCount) const;

	UPROPERTY(EditDefaultsOnly, Category="Stage")
	TSubclassOf<UStageInfoHUD> StageInfoHUDClass;
	
	UPROPERTY()
	TObjectPtr<UStageInfoHUD> StageInfoHUD;

	UPROPERTY(EditDefaultsOnly, Category="Stage")
	TSubclassOf<UStageWaitingTimer> StageWaitingTimerClass;
	
	UPROPERTY()
	TObjectPtr<UStageWaitingTimer> StageWaitingTimer;

	UPROPERTY(EditDefaultsOnly, Category="Stage")
	TSubclassOf<UStageStartAlert> StageStartAlertClass;
	
	UPROPERTY()
	TObjectPtr<UStageStartAlert> StageStartAlert;
};
