// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AuraWidgetController.generated.h"

class USpellConfig;
class UAttributeSet;
class UAbilitySystemComponent;
class UAuraAttributeSet;
class UAuraAbilitySystemComponent;
class AAuraPlayerState;
class AAuraPlayerController;

/**
 * Model 영역의 클래스들을 하나로 모으는 구조체
 */
USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet;
};

/**
 * Model(데이터가 저장된 클래스) 영역과 View(눈으로 볼 수 있는 영역, ie. widget) 영역 간의 데이터, 이벤트 전송을 위한 오브젝트
 * Widget이 WidgetController를 가지고(의존), WidgetController가 Model 영역의 클래스를 가진다(의존). => 단방향 의존 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()

public:
	// Model 영역의 클래스들을 저장
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);

	// Widget에 필요한 초기 값들을 Broadcast
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues() {}

	// Callback 함수 바인딩
	virtual void BindCallbacksToDependencies() {}
	
protected:
	UPROPERTY(BlueprintReadOnly, Category="Aura|Widget Controller")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category="Aura|Widget Controller")
	TObjectPtr<APlayerState> PlayerState;
	
	UPROPERTY(BlueprintReadOnly, Category="Aura|Widget Controller")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category="Aura|Widget Controller")
	TObjectPtr<UAttributeSet> AttributeSet;

	// CastChecked 수행해 반환
	AAuraPlayerController* GetAuraPlayerControllerChecked() const;
	AAuraPlayerState* GetAuraPlayerStateChecked() const;
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponentChecked() const;
	UAuraAttributeSet* GetAuraAttributeSetChecked() const;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Widget Controller")
	TObjectPtr<USpellConfig> SpellConfig;
};
