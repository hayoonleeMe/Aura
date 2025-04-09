// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/GameMenuType.h"
#include "MenuShortcutAlert.generated.h"

class UTextBlock;

/**
 * Menu를 여는 Key를 표시하는 Widget
 */
UCLASS()
class AURA_API UMenuShortcutAlert : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetText(EGameMenuType GameMenuType, const FKey& Key) const;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Menu;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Key;
};
