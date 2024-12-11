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

private:
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UAttributeConfig> AttributeConfig;

	// AttributeTag를 포함하는 AttributeInfo를 전달
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag,float AttributeValue) const;
};
