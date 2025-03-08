// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBar.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
class UProgressBar;

/**
 * 
 */
UCLASS()
class AURA_API UEnemyHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeHealthBar(UAbilitySystemComponent* ASC, const UAttributeSet* AS);
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

	float Health = 0.f;
	float MaxHealth = 0.f;

	void UpdateHealth(float NewValue);
	void UpdateMaxHealth(float NewValue);

	UFUNCTION()
	void OnOwnerDead();
};
