// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	check(OverlayWidgetClass);

	OverlayWidget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	const FWidgetControllerParams WidgetControllerParams{ PC, PS, ASC, AS };
	
	if (OverlayWidget && GetOverlayWidgetController(WidgetControllerParams))
	{
		// OverlayWidget에 WidgetController 저장 및 초기값 Broadcast
		OverlayWidget->SetWidgetController(OverlayWidgetController);
		OverlayWidgetController->BroadcastInitialValues();
		// 화면에 표시
		OverlayWidget->AddToViewport();
	}

	check(AttributeMenuWidgetClass);
	
	// AttributeMenuWidget을 생성해 캐싱
	AttributeMenuWidget = CreateWidget<UAuraUserWidget>(GetWorld(), AttributeMenuWidgetClass);
	if (AttributeMenuWidget && GetAttributeMenuWidgetController(WidgetControllerParams))
	{
		// AttributeMenuWidget에 WidgetController 저장 및 초기값 Broadcast
		AttributeMenuWidget->SetWidgetController(AttributeMenuWidgetController);
		// 화면에 추가하고 숨기기
		AttributeMenuWidget->AddToViewport();
		AttributeMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
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
	return OverlayWidgetController;
}

UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
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
	return AttributeMenuWidgetController;
}

void AAuraHUD::SetAttributeMenuVisible(bool bVisible) const
{
	if (AttributeMenuWidget && AttributeMenuWidgetController)
	{
		// Attribute 값들을 업데이트하고 화면에 표시
		AttributeMenuWidgetController->BroadcastInitialValues();
		AttributeMenuWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}
