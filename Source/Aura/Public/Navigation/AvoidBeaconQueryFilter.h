// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "AvoidBeaconQueryFilter.generated.h"

/**
 * @brief UNavArea_Beacon을 피하도록 설정된 Query Filter
 * 
 * @details UNavArea_Beacon을 사용하는 UNavModifierComponent에 영향을 받는 NavMesh 영역을 피할 때 사용한다.
 * UNavigationSystemV1::FindPathToLocationSynchronously()와 같은 함수에서 사용할 수 있다.
 */
UCLASS()
class AURA_API UAvoidBeaconQueryFilter : public UNavigationQueryFilter
{
	GENERATED_BODY()

public:
	UAvoidBeaconQueryFilter();
};
