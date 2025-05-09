// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FramedValue.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class AURA_API UFramedValue : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Value;
};
