// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UAuraAbilitySystemComponent;
class UInputMappingContext;
class UAuraInputConfig;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
private:
	/*
	 *	Input
	 */
	UPROPERTY(EditAnywhere, Category="Aura|Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<UAuraInputConfig> AuraInputConfig;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	/*
	 *	GAS
	 */
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();
};
