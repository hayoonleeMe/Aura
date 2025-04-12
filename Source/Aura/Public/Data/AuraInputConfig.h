// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AuraInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FAbilityInputMapping
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<const UInputAction> InputAction;

	UPROPERTY(EditDefaultsOnly, meta=(Categories="InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly)
	int32 InputID{INDEX_NONE};
};

/**
 * Ability에 대한 InputAction, InputTag 쌍의 배열을 저장하는 DataAsset 
 */
UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FAbilityInputMapping> AbilityInputActions;

	int32 GetInputIDForInputTag(const FGameplayTag& InputTag);
	FGameplayTag GetInputTagForInputID(int32 InputID);
	const UInputAction* GetInputActionForInputTag(const FGameplayTag& InputTag) const;
};
