// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

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

	// OverlayWidgetController가 유효하지 않다면 FWidgetControllerParams로 OverlayWidgetController를 생성해 반환
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);

	// AttributeMenuWidgetController가 유효하지 않다면 FWidgetControllerParams로 AttributeMenuWidgetController를 생성해 반환
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);

	// Attribute Menu를 화면에 표시하거나 숨김
	void SetAttributeMenuVisible(bool bVisible) const;

private:
	/*
	 *	Overlay
	 */
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Overlay")
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Overlay")
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	// ============================================================================
	// Attribute Menu
	// ============================================================================

	// 위젯을 캐싱하고 SetAttributeMenuVisible()로 화면에 표시하거나 숨김
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> AttributeMenuWidget;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Attribute Menu")
	TSubclassOf<UAuraUserWidget> AttributeMenuWidgetClass;

	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Attribute Menu")
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
};
