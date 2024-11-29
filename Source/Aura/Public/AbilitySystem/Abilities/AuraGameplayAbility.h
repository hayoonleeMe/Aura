// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	// Ability를 활성화하는 Input Key를 나타내는 Tag
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	FGameplayTag InputTag;

	// Ability를 실행하면 재생할 기본 AnimMontage
	UPROPERTY(EditDefaultsOnly, Category="Aura|Montage")
	TObjectPtr<UAnimMontage> BaseMontage;
};
