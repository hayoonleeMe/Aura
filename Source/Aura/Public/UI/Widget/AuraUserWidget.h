// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

class UAuraAttributeSet;
class AAuraPlayerState;
class AAuraGameStateBase;
class UAuraAbilitySystemComponent;

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
	UAuraAbilitySystemComponent* GetOwnerAuraAbilitySystemComponentChecked() const;
	UAuraAttributeSet* GetOwnerAuraAttributeSetChecked() const;
	AAuraGameStateBase* GetAuraGameStateBaseChecked() const;

	// 게임에서 기본으로 사용하는 Input Mode로 설정
	void SetInGameInputMode() const;

	// UI 전용 Input Mode로 설정
	void SetUIInputMode(UUserWidget* WidgetToFocus = nullptr) const;

	FOnRemovedSignature OnRemovedDelegate;

protected:
	virtual void NativeDestruct() override;
	
	virtual void BroadcastInitialValues() {}
};
