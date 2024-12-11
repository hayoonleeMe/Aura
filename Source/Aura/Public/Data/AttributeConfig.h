// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AttributeConfig.generated.h"

/**
 * UI에 Attribute를 표시하기 위한 정보를 저장하는 구조체
 */
USTRUCT(BlueprintType)
struct FAttributeInfo
{
	GENERATED_BODY()

	FAttributeInfo()
		: Value(0.f)
	{}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttributeTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription;
	
	UPROPERTY(BlueprintReadOnly)
	float Value;
};

/**
 * UI에 Attribute를 표시하기 위한 정보를 저장하는 DataAsset
 */
UCLASS()
class AURA_API UAttributeConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAttributeInfo> AttributeInfos;

	// GameplayTag를 포함하는 FAttributeInfo를 반환
	FAttributeInfo GetAttributeInfoByTag(const FGameplayTag& Tag) const;
};
