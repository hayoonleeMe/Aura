// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerNameplate.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class AURA_API UPlayerNameplate : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindW))
	TObjectPtr<UTextBlock> Text_PlayerName;

	// PlayerName TextBlock 설정
	void SetPlayerName(const FString& PlayerName) const;
};
