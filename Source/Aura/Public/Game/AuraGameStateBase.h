// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AuraGameStateBase.generated.h"

class UAuraInputConfig;
class UInputMappingContext;

/**
 * 
 */
UCLASS()
class AURA_API AAuraGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UAuraInputConfig> AuraInputConfig;
};
