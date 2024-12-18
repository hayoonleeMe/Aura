// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

// AuraPlayerState의 SpellPoints 값이 변경되면 그 값을 전달하는 델레게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellPointsChangedSignature, int32, Value);

/**
 * Spell Menu에서 사용할 Widget Controller
 */
UCLASS()
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FOnSpellPointsChangedSignature OnSpellPointsChangedDelegate;
};
