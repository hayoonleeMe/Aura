// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UGameOverlay;
class UAuraUserWidget;
class UAttributeSet;
class UAbilitySystemComponent;

/**
 * Widget을 화면에 표시하고 관리하는 HUD Class
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()

public:
	// Overlay Widget을 초기화하고 화면에 표시
	void InitOverlay();

private:
	// ============================================================================
	// Overlay
	// ============================================================================

	UPROPERTY()
	TObjectPtr<UGameOverlay> GameOverlay;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Overlay")
	TSubclassOf<UGameOverlay> GameOverlayClass;
};
