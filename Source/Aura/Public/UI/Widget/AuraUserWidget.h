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
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// UI Input Mapping Context를 사용할 것인지 여부
	bool bUseUIMapping = false;

	// AAuraPlayerController에 UI IMC를 추가하고, Ability IMC를 제거한다.
	// bUseUIMapping가 true일 때 NativeConstruct()에서 호출된다. 
	void AddUIMappingContext();

	// AAuraPlayerController에 UI IMC를 제거하고, 다시 Ability IMC를 추가한다.
	// bUseUIMapping가 true일 때 NativeDestruct()에서 호출된다.
	void RemoveUIMappingContext();
};
