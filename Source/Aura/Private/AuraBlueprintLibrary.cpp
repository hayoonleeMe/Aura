// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraBlueprintLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectTypes.h"
#include "Game/AuraGameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Types/AuraAbilityTypes.h"

UEnemyClassConfig* UAuraBlueprintLibrary::GetEnemyClassConfig(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	return IsValid(AuraGameModeBase) ? AuraGameModeBase->EnemyClassConfig : nullptr;
}

UCurveTable* UAuraBlueprintLibrary::GetDamageCalculationCoefficients(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	return IsValid(AuraGameModeBase) ? AuraGameModeBase->DamageCalculationCoefficients : nullptr;
}

void UAuraBlueprintLibrary::ApplyDamageEffect(const FDamageEffectParams& Params)
{
	const AActor* SourceAvatarActor = Params.SourceAbilitySystemComponent->GetAvatarActor();
	FGameplayEffectContextHandle EffectContextHandle = Params.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	
	const FGameplayEffectSpecHandle EffectSpecHandle = Params.SourceAbilitySystemComponent->MakeOutgoingSpec(Params.DamageEffectClass, Params.AbilityLevel, EffectContextHandle);
	// DamageTypeTag에 BaseDamage를 SetByCaller로 등록한다.
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Params.DamageTypeTag, Params.BaseDamage);
	
	Params.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);
}

bool UAuraBlueprintLibrary::IsNotFriend(const AActor* Actor1, const AActor* Actor2)
{
	const ICombatInterface* CombatInterface1 = Cast<ICombatInterface>(Actor1);
	const ICombatInterface* CombatInterface2 = Cast<ICombatInterface>(Actor2);
	if (CombatInterface1 && CombatInterface2)
	{
		const FGameplayTag& RoleTag1 = CombatInterface1->GetRoleTag();
		const FGameplayTag& RoleTag2 = CombatInterface2->GetRoleTag();
		return !RoleTag1.MatchesTagExact(RoleTag2);
	}
	return true;
}

void UAuraBlueprintLibrary::ExecuteGameplayCue(AActor* OwnerActor, const FGameplayTag& CueTag, const FVector& CueLocation)
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor))
	{
		FGameplayCueParameters CueParameters;
		CueParameters.Location = CueLocation;
		ASC->ExecuteGameplayCue(CueTag, CueParameters);
	}
}

void UAuraBlueprintLibrary::GetAlivePawnsFromPlayers(const UObject* WorldContextObject, TArray<AActor*>& OutPlayers)
{
	if (AGameStateBase* GameState = WorldContextObject->GetWorld()->GetGameState())
	{
		for (const APlayerState* PS : GameState->PlayerArray)
		{
			const ICombatInterface* CombatInterface = Cast<ICombatInterface>(PS->GetPawn());
			if (PS && IsValid(PS->GetPawn()) && CombatInterface && !CombatInterface->IsDead())
			{
				OutPlayers.Add(PS->GetPawn());
			}
		}
	}
}
