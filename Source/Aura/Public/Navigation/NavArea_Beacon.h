// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavAreas/NavArea.h"
#include "NavArea_Beacon.generated.h"

/**
 * Beacon NavArea
 *
 * UNavArea_Default와 동일하다.
 */
UCLASS()
class AURA_API UNavArea_Beacon : public UNavArea
{
	GENERATED_BODY()

public:
	UNavArea_Beacon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
