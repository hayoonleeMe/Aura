// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageIndicatorComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UDamageIndicatorComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UDamageIndicatorComponent();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateDamage(float Damage);

	// Widget을 표시할 위치를 구하는 데 사용할 최소 거리
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	float WidgetSpawnMinDist;

	// Widget을 표시할 위치를 구하는 데 사용할 최대 거리
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	float WidgetSpawnMaxDist;
};
