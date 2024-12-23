// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

public:
	// PlayerState의 Spell Points 반환
	virtual int32 GetSpellPoints() const { return 0; }

	// PlayerState의 SpellPoints를 1 증가
	virtual void IncrementSpellPoints() {}

	// PlayerState의 SpellPoints를 1 감소
	virtual void DecrementSpellPoints() {}
};
