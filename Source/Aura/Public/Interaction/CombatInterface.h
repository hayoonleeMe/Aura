// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

/**
 * GameplayTag에 연관된 Combat 정보들을 저장하는 구조체
 */
USTRUCT(BlueprintType)
struct FTaggedCombatInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag Tag;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> AnimMontage;

	UPROPERTY(EditDefaultsOnly)
	FName CombatSocketName;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	int32 GetCharacterLevel();

	UFUNCTION(BlueprintNativeEvent)
	FGameplayTag GetRoleTag() const;

	UFUNCTION(BlueprintNativeEvent)
	void SetFacingTarget(const FVector& TargetLocation);

	virtual void Die() {}

	UFUNCTION(BlueprintNativeEvent)
	bool IsDead() const;
	
	UFUNCTION(BlueprintNativeEvent)
	void GetAttackCheckRange(float& OutRadius, float& OutHalfHeight) const;
	
	UFUNCTION(BlueprintNativeEvent)
	FTaggedCombatInfo GetTaggedCombatInfo(const FGameplayTag& InTag) const;

	UFUNCTION(BlueprintNativeEvent)
	FVector GetCombatSocketLocation(const FName& CombatSocketName) const;
	
	UFUNCTION(BlueprintNativeEvent)
	FTransform GetCombatSocketTransform(const FName& CombatSocketName) const;
	
	UFUNCTION(BlueprintNativeEvent)
	void SetCombatTarget(AActor* InCombatTarget);

	virtual TWeakObjectPtr<AActor> GetCombatTarget() const { return nullptr; }
};
