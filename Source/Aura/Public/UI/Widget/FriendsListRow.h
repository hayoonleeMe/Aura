// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FriendsListRow.generated.h"

class USquareButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class AURA_API UFriendsListRow : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Name;

	FString PlayerName;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USquareButton> Button_Invite;

	UFUNCTION()
	void OnInviteButtonClicked();

	FUniqueNetIdWrapper UserId;
};
