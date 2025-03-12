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
	void AddOptionsMenu() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UOptionsMenu> OptionsMenuWidgetClass;
};
