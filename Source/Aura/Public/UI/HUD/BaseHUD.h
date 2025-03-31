// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseHUD.generated.h"

class UOptionsMenu;

/**
 * 
 */
UCLASS()
class AURA_API ABaseHUD : public AHUD
{
	GENERATED_BODY()

public:
	void RegisterWidget(UObject* Object);
	void UnregisterCurrentWidget();
	void CloseCurrentWidget();
	void AddOptionsMenu() const;

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<UObject>> CurrentWidgetStack;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UOptionsMenu> OptionsMenuWidgetClass;
};
