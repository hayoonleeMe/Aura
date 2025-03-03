// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpellDescription.generated.h"

class URichTextBlock;

/**
 * 
 */
UCLASS()
class AURA_API USpellDescription : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URichTextBlock> RichText_Description;
};
