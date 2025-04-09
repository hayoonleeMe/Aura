// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseHUD.h"
#include "Types/GameMenuType.h"
#include "AuraHUD.generated.h"

class UGameOverlay;
class UAttributeSet;
class UAbilitySystemComponent;

/**
 * Widget을 화면에 표시하고 관리하는 HUD Class
 */
UCLASS()
class AURA_API AAuraHUD : public ABaseHUD
{
	GENERATED_BODY()

public:
	// Overlay Widget을 초기화하고 화면에 표시
	void InitOverlay();

	// GameMenuType에 해당하는 메뉴를 연다.
	void OpenMenu(EGameMenuType GameMenuType) const;

	// GameOverlay의 RenderOpacity를 변경해 표시하거나 숨긴다.
	void ShowGameOverlay(bool bShow) const;

	// 모든 메뉴의 단축키를 알리는 Alert Widget을 표시
	void ShowAllMenuShortcutAlert(const TArray<TTuple<EGameMenuType, FKey>>& MenuKeys) const;

private:
	// ============================================================================
	// Overlay
	// ============================================================================

	UPROPERTY()
	TObjectPtr<UGameOverlay> GameOverlay;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Overlay")
	TSubclassOf<UGameOverlay> GameOverlayClass;
};
