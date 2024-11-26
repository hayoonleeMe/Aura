// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/EnemyClassType.h"
#include "EnemyClassConfig.generated.h"

class UGameplayAbility;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FEnemyClassInfo
{
	GENERATED_BODY()

	// Enemy의 Primary Attributes 초기화 Gameplay Effect
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	// Enemy 어빌리티
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;		
};

/**
 * Enemy Class Type에 따른 Enemy 초기화 정보를 저장하는 DataAsset
 */
UCLASS()
class AURA_API UEnemyClassConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<EEnemyClassType, FEnemyClassInfo> TypeToInfoMap;

	// Enemy의 Secondary Attributes 초기화 Gameplay Effect
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	
	// Enemy의 Health, Mana 초기화 Gameplay Effect
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> VitalAttributes;

	// Enemy 공통 어빌리티
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> SharedAbilities;
};
