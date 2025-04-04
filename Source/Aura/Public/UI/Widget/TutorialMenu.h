// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ClosableWidget.h"
#include "TutorialMenu.generated.h"

class USquareButton;

/**
 * 
 */
UCLASS()
class AURA_API UTutorialMenu : public UClosableWidget
{
	GENERATED_BODY()

public:
	/* Begin IClosableWidgetInterface */
	virtual void CloseMenu() override;
	/* End IClosableWidgetInterface */

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_Close;
};
