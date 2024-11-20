// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

class UAuraInputConfig;
/**
 *
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	// UAuraInputConfig DataAsset에 저장된 모든 InputAction에 대해 Pressed, Released, Held Event에 대한 Callback을 바인딩한다.
	template <class UserClass, typename PressedFunction, typename ReleasedFunction, typename HeldFunction>
	void BindAbilityActions(UAuraInputConfig* InputConfig, UserClass* Object, PressedFunction PressedFunc, ReleasedFunction ReleasedFunc, HeldFunction HeldFunc);
};

template <class UserClass, typename PressedFunction, typename ReleasedFunction, typename HeldFunction>
void UAuraInputComponent::BindAbilityActions(UAuraInputConfig* InputConfig, UserClass* Object, PressedFunction PressedFunc, ReleasedFunction ReleasedFunc, HeldFunction HeldFunc)
{
	check(InputConfig);
	
	for (const FAbilityInputMapping& Mapping : InputConfig->AbilityInputActions)
	{
		if (IsValid(Mapping.InputAction) && Mapping.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Mapping.InputAction, ETriggerEvent::Started, Object, PressedFunc, Mapping.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(Mapping.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Mapping.InputTag);
			}
			if (HeldFunc)
			{
				BindAction(Mapping.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Mapping.InputTag);
			}
		}
	}
}
