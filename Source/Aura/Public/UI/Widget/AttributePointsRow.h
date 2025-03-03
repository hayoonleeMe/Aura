// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributePointsRow.generated.h"

class UFramedValue;

/**
 * 
 */
UCLASS()
class AURA_API UAttributePointsRow : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UFramedValue> FramedValue;
};
