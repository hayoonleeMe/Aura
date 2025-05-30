// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/AuraInputConfig.h"
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
	// UAuraInputConfig DataAsset에 저장된 모든 InputAction에 대해 Pressed, Released Event에 대한 Callback을 바인딩한다.
	template <class UserClass, typename PressedFunction, typename ReleasedFunction>
	void BindAbilityActions(UAuraInputConfig* InputConfig, UserClass* Object, PressedFunction PressedFunc, ReleasedFunction ReleasedFunc);
};

template <class UserClass, typename PressedFunction, typename ReleasedFunction>
void UAuraInputComponent::BindAbilityActions(UAuraInputConfig* InputConfig, UserClass* Object, PressedFunction PressedFunc, ReleasedFunction ReleasedFunc)
{
	for (const FAbilityInputMapping& Mapping : InputConfig->AbilityInputActions)
	{
		if (Mapping.InputAction && Mapping.InputTag.IsValid() && Mapping.InputID != INDEX_NONE)
		{
			if (PressedFunc)
			{
				BindAction(Mapping.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Mapping.InputID, true);
				BindAction(Mapping.InputAction, ETriggerEvent::Started, Object, PressedFunc, Mapping.InputID, false);
			}
			if (ReleasedFunc)
			{
				BindAction(Mapping.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Mapping.InputID);
			}
		}
	}
}
