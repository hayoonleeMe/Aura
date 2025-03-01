// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobeProgressBar.h"
#include "HealthGlobe.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UHealthGlobe : public UGlobeProgressBar
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void BroadcastInitialValues() override;

public:
	void UpdateHealth(float NewValue);
	void UpdateMaxHealth(float NewValue);
	
	float Health = 0.f;
	float MaxHealth = 0.f;
};
