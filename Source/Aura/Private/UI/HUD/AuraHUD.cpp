// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	// Widget Controller 캐싱
	const FWidgetControllerParams WidgetControllerParams{ PC, PS, ASC, AS };
	MakeOverlayWidgetController(WidgetControllerParams);
	MakeAttributeMenuWidgetController(WidgetControllerParams);
	MakeSpellMenuWidgetController(WidgetControllerParams);
	
	check(OverlayWidgetClass);

	OverlayWidget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	if (OverlayWidget && OverlayWidgetController)
	{
		// OverlayWidget에 WidgetController 저장 및 초기값 Broadcast
		OverlayWidget->SetWidgetController(OverlayWidgetController);
		// 화면에 표시
		OverlayWidget->AddToViewport();
	}
}

void AAuraHUD::OnStageStatusChanged(EStageStatus StageStatus, int32 StageNumber, double WaitingTimerEndSeconds, int32 TotalEnemyCount) const
{
	if (OverlayWidgetController)
	{
		OverlayWidgetController->OnStageStatusChanged(StageStatus, StageNumber, WaitingTimerEndSeconds, TotalEnemyCount);
	}
}

void AAuraHUD::OnEnemyDead() const
{
	if (OverlayWidgetController)
	{
		OverlayWidgetController->OnEnemyDead();
	}
}

void AAuraHUD::MakeOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	check(OverlayWidgetControllerClass);

	if (!OverlayWidgetController)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		if (OverlayWidgetController)
		{
			// WidgetController 초기화 및 바인딩
			OverlayWidgetController->SetWidgetControllerParams(WCParams);
			OverlayWidgetController->BindCallbacksToDependencies();
		}
	}
}

void AAuraHUD::MakeAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	check(AttributeMenuWidgetControllerClass);

	if (!AttributeMenuWidgetController)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		if (AttributeMenuWidgetController)
		{
			// WidgetController 초기화 및 바인딩
			AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
			AttributeMenuWidgetController->BindCallbacksToDependencies();
		}
	}
}

void AAuraHUD::MakeSpellMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	check(SpellMenuWidgetControllerClass);
    
    if (!SpellMenuWidgetController)
    {
    	SpellMenuWidgetController = NewObject<USpellMenuWidgetController>(this, SpellMenuWidgetControllerClass);
    	if (SpellMenuWidgetController)
    	{
    		// WidgetController 초기화 및 바인딩
    		SpellMenuWidgetController->SetWidgetControllerParams(WCParams);
    		SpellMenuWidgetController->BindCallbacksToDependencies();
    	}
    }
}
