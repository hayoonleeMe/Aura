// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OffensiveSpellTree.generated.h"

class USpellGlobeButton;

/**
 * 
 */
UCLASS()
class AURA_API UOffensiveSpellTree : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobeButton> GlobeButton_1;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobeButton> GlobeButton_2;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobeButton> GlobeButton_3;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobeButton> GlobeButton_4;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobeButton> GlobeButton_5;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobeButton> GlobeButton_6;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobeButton> GlobeButton_7;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobeButton> GlobeButton_8;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpellGlobeButton> GlobeButton_9;
};
