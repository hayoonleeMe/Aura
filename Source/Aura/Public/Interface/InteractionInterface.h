// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Cursor로 선택할 수 있는 Actor와의 상호작용을 구현하는 인터페이스
 */
class AURA_API IInteractionInterface
{
	GENERATED_BODY()

public:
	// Highlight Actor
	virtual void HighlightActor() {}

	// UnHighlight Actor
	virtual void UnHighlightActor() {}

	// Interact with Actor
	virtual void Interact() {}

	// 상호작용 할 액터까지 이동할 때 목적지 도착 인정 범위를 Override 하기 위해 구현해야 함
	virtual float GetOverrideArriveAcceptanceRadius() const { return 0.f; }

	virtual bool CanTryInteract() const = 0;
};
