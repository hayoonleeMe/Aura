// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PassiveSpellTree.generated.h"

class USpellGlobeButton;

/**
 * 
 */
UCLASS()
class AURA_API UPassiveSpellTree : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobeButton> GlobeButton_HaloOfProtection;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobeButton> GlobeButton_HealthSiphon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobeButton> GlobeButton_ManaSiphon;
};
