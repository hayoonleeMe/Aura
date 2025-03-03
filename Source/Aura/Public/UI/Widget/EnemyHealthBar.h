// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraUserWidget.h"
#include "EnemyHealthBar.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class AURA_API UEnemyHealthBar : public UAuraUserWidget
{
	GENERATED_BODY()

protected:
	virtual void BroadcastInitialValues() override;

public:
	void InitializeHealthBar(AActor* OwnerEnemy);
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

	float Health = 0.f;
	float MaxHealth = 0.f;

	void UpdateHealth(float NewValue);
	void UpdateMaxHealth(float NewValue);

	UFUNCTION()
	void OnOwnerDead();
};
