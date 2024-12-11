// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

struct FGameplayTag;
class UAttributeConfig;
struct FAttributeInfo;

// Attribute가 변경되면 그 Attribute의 정보를 담은 구조체를 전달하는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedWithInfoSignature, const FAttributeInfo&, AttributeInfo);

// AuraPlayerState의 AttributePoints 값이 변경되면 그 값을 전달하는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributePointsChangedSignature, int32, Value);

/**
 * Attribute Menu에서 사용할 Widget Controller
 */
UCLASS()
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedWithInfoSignature OnAttributeChangedWithInfoDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnAttributePointsChangedSignature OnAttributePointsChangedDelegate;

	// AttributeTag에 해당하는 Attribute의 값을 1만큼 증가
	UFUNCTION(BlueprintCallable)
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

private:
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UAttributeConfig> AttributeConfig;

	// AttributeTag를 포함하는 AttributeInfo를 전달
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag,float AttributeValue) const;
};
