// Fill out your copyright notice in the Description page of Project Settings.


#include "Navigation/AvoidBeaconQueryFilter.h"

#include "Navigation/NavArea_Beacon.h"

UAvoidBeaconQueryFilter::UAvoidBeaconQueryFilter()
{
	FNavigationFilterArea Area;
	Area.AreaClass = UNavArea_Beacon::StaticClass();
	Area.bIsExcluded = true;	// Avoid
	Areas.Add(Area);
}
