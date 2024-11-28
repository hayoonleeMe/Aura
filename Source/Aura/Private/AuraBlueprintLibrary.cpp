// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraBlueprintLibrary.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"

UEnemyClassConfig* UAuraBlueprintLibrary::GetEnemyClassConfig(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	return IsValid(AuraGameModeBase) ? AuraGameModeBase->EnemyClassConfig : nullptr;
}


