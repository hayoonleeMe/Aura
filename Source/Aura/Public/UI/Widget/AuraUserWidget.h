// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

// 위젯이 제거될 때 호출되는 델레게이트
DECLARE_MULTICAST_DELEGATE(FOnRemovedSignature);

/**
 * 
 */
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnRemovedSignature OnRemovedDelegate;

protected:
	virtual void NativeDestruct() override;
};
