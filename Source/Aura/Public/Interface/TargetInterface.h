// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TargetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Cursor로 선택할 수 있는 Target Actor가 구현할 수 있는 Interface
 */
class AURA_API ITargetInterface
{
	GENERATED_BODY()

public:
	// Highlight Target Actor
	virtual void HighlightActor() {}

	// UnHighlight Target Actor
	virtual void UnHighlightActor() {}
};
