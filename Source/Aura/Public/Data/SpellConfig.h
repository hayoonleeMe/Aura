// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SpellConfig.generated.h"

class UGameplayAbility;

/**
 * UI에 Spell을 표시하기 위한 정보를 저장하는 구조체
 */
USTRUCT(BlueprintType)
struct FSpellInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SpellTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> SpellAbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> SpellIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> Background;
};

/**
 * UI에 Spell을 표시하기 위한 정보를 저장하는 DataAsset
 */
UCLASS()
class AURA_API USpellConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FSpellInfo> SpellInfos;

	// Tag를 포함하는 FSpellInfo 반환
	FSpellInfo GetSpellInfoByTag(const FGameplayTag& Tag) const;
};
