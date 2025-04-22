// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraAbility_RangeShaman.h"

#include "AuraGameplayTags.h"

UAuraAbility_RangeShaman::UAuraAbility_RangeShaman()
{
	DamageTypeTag = AuraGameplayTags::Damage_Type_Fire;
	bNeedCursorTargetHitResult = false;
}
