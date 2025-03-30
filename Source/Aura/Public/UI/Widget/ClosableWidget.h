// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/ClosableWidgetInterface.h"
#include "ClosableWidget.generated.h"

/**
 * 닫을 수 있는 위젯을 나타내는 Abstract class
 */
UCLASS(Abstract)
class AURA_API UClosableWidget : public UUserWidget, public IClosableWidgetInterface
{
	GENERATED_BODY()

public:
	/* Begin IClosableWidgetInterface */
	virtual void CloseMenu() override;
	virtual FOnRemovedSignature& GetOnRemovedDelegate() override { return OnRemovedDelegate; }
	/* End IClosableWidgetInterface */

protected:
	virtual void NativeConstruct() override;

	FOnRemovedSignature OnRemovedDelegate;
};
