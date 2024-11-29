// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// FAuraGameplayTags 초기화
	FAuraGameplayTags::InitializeNativeGameplayTags();

	// for TargetData (TargetDataScriptStructCache 초기화를 포함해 여러 초기화 수행)
	UAbilitySystemGlobals::Get().InitGlobalData();
}
