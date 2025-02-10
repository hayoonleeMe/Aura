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
};
