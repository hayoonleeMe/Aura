// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraBlueprintLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "EngineUtils.h"
#include "GameplayEffectTypes.h"
#include "LevelSequenceActor.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraGameplayAbilityTargetData_SingleTargetHit.h"
#include "AbilitySystem/AuraGameplayEffectContext.h"
#include "Game/AuraGameModeBase.h"
#include "Game/AuraGameStateBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Interface/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "Types/DamageEffectParams.h"
#include "UI/HUD/AuraHUD.h"

UEnemyClassConfig* UAuraBlueprintLibrary::GetEnemyClassConfig(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	return IsValid(AuraGameModeBase) ? AuraGameModeBase->EnemyClassConfig : nullptr;
}

float UAuraBlueprintLibrary::GetArmorPenetrationCoefficientByLevel(const UObject* WorldContextObject, float Level)
{
	if (const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return AuraGameModeBase->ArmorPenetrationCoefficientCurve.GetValueAtLevel(Level);
	}
	return 0.f;
}

float UAuraBlueprintLibrary::GetEffectiveArmorCoefficientByLevel(const UObject* WorldContextObject, float Level)
{
	if (const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return AuraGameModeBase->EffectiveArmorCoefficientCurve.GetValueAtLevel(Level);
	}
	return 0.f;
}

float UAuraBlueprintLibrary::GetCriticalHitResistanceCoefficientByLevel(const UObject* WorldContextObject, float Level)
{
	if (const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return AuraGameModeBase->CriticalHitResistanceCoefficientCurve.GetValueAtLevel(Level);
	}
	return 0.f;
}

bool UAuraBlueprintLibrary::ApplyDamageEffect(const FDamageEffectParams& Params)
{
	check(Params.TargetAbilitySystemComponent);

	// 데미지 무적
	if (Params.TargetAbilitySystemComponent->HasMatchingGameplayTag(AuraGameplayTags::Gameplay_Invincibility))
	{
		return false;
	}
	
	// Set Debuff Parameters
	const FGameplayEffectContextHandle EffectContextHandle = Params.SourceAbilitySystemComponent->MakeEffectContext();
	if (FAuraGameplayEffectContext* AuraEffectContext = FAuraGameplayEffectContext::ExtractEffectContext(EffectContextHandle))
	{
		AuraEffectContext->SetDebuffTag(Params.DebuffTag);
		AuraEffectContext->SetDebuffChance(Params.DebuffChance);
	}
	const FGameplayEffectSpecHandle EffectSpecHandle = Params.SourceAbilitySystemComponent->MakeOutgoingSpec(Params.DamageEffectClass, Params.AbilityLevel, EffectContextHandle);
	
	// DamageTypeTag에 BaseDamage를 SetByCaller로 등록한다.
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Params.DamageTypeTag, Params.BaseDamage);
	
	Params.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);

	return true;
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

void UAuraBlueprintLibrary::ExecuteGameplayCueWithParams(AActor* OwnerActor, const FGameplayTag& CueTag, const FGameplayCueParameters& CueParameters)
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor))
	{
		ASC->ExecuteGameplayCue(CueTag, CueParameters);
	}
}

FVector UAuraBlueprintLibrary::GetCombatSocketLocationForTag(AActor* OwnerActor, FGameplayTag Tag)
{
	if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(OwnerActor))
	{
		const FName CombatSocketName = CombatInterface->GetTaggedCombatInfo(Tag).CombatSocketName;
		return CombatInterface->GetCombatSocketLocation(CombatSocketName);
	}
	return FVector::ZeroVector;
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

void UAuraBlueprintLibrary::GetEnemiesOverlappedByChannel(const UWorld* World, TArray<AActor*>& OutEnemies, const FVector& Pos, const FQuat& Rot,
	ECollisionChannel TraceChannel, const FCollisionShape& CollisionShape)
{
	if (!World)
	{
		return;
	}
	
	const FGameplayTag EnemyRoleTag = AuraGameplayTags::Role_Enemy;
	
	TArray<FOverlapResult> OverlapResults;
	World->OverlapMultiByChannel(OverlapResults, Pos, Rot, TraceChannel, CollisionShape);

	for (const FOverlapResult& Result : OverlapResults)
	{
		if (const ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(Result.GetActor()))
		{
			// 살아있는 Enemy를 Array에 추가
			if (!TargetCombatInterface->IsDead() && TargetCombatInterface->GetRoleTag().MatchesTagExact(EnemyRoleTag))
			{
				OutEnemies.Add(Result.GetActor());
			}
		}
	}
}

FVector UAuraBlueprintLibrary::GetActorFeetLocation(const AActor* Actor)
{
	if (const ACharacter* Character = Cast<ACharacter>(Actor))
	{
		return Character->GetNavAgentLocation();
	}
	return FVector::ZeroVector;
}

void UAuraBlueprintLibrary::SetInGameInputMode(const UObject* WorldContextObject)
{
	if (WorldContextObject && WorldContextObject->GetWorld())
	{
		if (AAuraPlayerController* AuraPC = WorldContextObject->GetWorld()->GetFirstPlayerController<AAuraPlayerController>())
		{
			AuraPC->SetInGameInputMode();
		}
	}
}

void UAuraBlueprintLibrary::SetUIInputMode(const UObject* WorldContextObject)
{
	if (WorldContextObject && WorldContextObject->GetWorld())
	{
		if (AAuraPlayerController* AuraPC = WorldContextObject->GetWorld()->GetFirstPlayerController<AAuraPlayerController>())
		{
			AuraPC->SetUIInputMode();
		}
	}
}

int32 UAuraBlueprintLibrary::GetLevelAttributeValue(AActor* Actor)
{
	if (const UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor)))
	{
		if (const UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AuraASC->GetAttributeSet(UAuraAttributeSet::StaticClass())))
		{
			return AuraAS->GetLevel();
		}
	}
	return 0;
}

int32 UAuraBlueprintLibrary::GetLevelUpXPRequirement(const APlayerController* OwningController, int32 Level)
{
	if (OwningController)
	{
		if (const AAuraPlayerState* AuraPS = OwningController->GetPlayerState<AAuraPlayerState>())
		{
			return AuraPS->GetLevelUpXPRequirement(Level);
		}
	}
	return 0;
}

TWeakObjectPtr<AActor> UAuraBlueprintLibrary::GetCursorTargetFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index)
{
	if (TargetData.Data.IsValidIndex(Index))
	{
		if (const FAuraGameplayAbilityTargetData_SingleTargetHit* Data = static_cast<FAuraGameplayAbilityTargetData_SingleTargetHit*>(TargetData.Data[Index].Get()))
		{
			return Data->CursorTarget;
		}
	}

	return nullptr;
}

ALevelSequenceActor* UAuraBlueprintLibrary::GetLevelSequenceActorMatchingTag(const UWorld* World, const FName& Tag)
{
	for (TActorIterator<ALevelSequenceActor> It(World); It; ++It)
	{
		if (It && It->ActorHasTag(Tag))
		{
			return *It;
		}
	}
	return nullptr;
}

ALevelSequenceActor* UAuraBlueprintLibrary::GetPauseMenuLevelSequenceActor(const UObject* WorldContextObject)
{
	const UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	const AAuraGameStateBase* AuraGameStateBase = World ? World->GetGameState<AAuraGameStateBase>() : nullptr;
	return AuraGameStateBase ? AuraGameStateBase->PauseMenuLevelSequenceActor : nullptr;
}

UOverlayWidgetController* UAuraBlueprintLibrary::GetOverlayWidgetController(const APlayerController* OwningController)
{
	if (OwningController)
	{
		if (const AAuraHUD* AuraHUD = OwningController->GetHUD<AAuraHUD>())
		{
			return AuraHUD->GetOverlayWidgetController();
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraBlueprintLibrary::GetAttributeMenuWidgetController(const APlayerController* OwningController)
{
	if (OwningController)
	{
		if (const AAuraHUD* AuraHUD = OwningController->GetHUD<AAuraHUD>())
		{
			return AuraHUD->GetAttributeMenuWidgetController();
		}
	}
	return nullptr;
}

USpellMenuWidgetController* UAuraBlueprintLibrary::GetSpellMenuWidgetController(const APlayerController* OwningController)
{
	if (OwningController)
	{
		if (const AAuraHUD* AuraHUD = OwningController->GetHUD<AAuraHUD>())
		{
			return AuraHUD->GetSpellMenuWidgetController();
		}
	}
	return nullptr;
}

void UAuraBlueprintLibrary::GetSpreadDirections(TArray<FVector>& OutDirections, int32 NumDirections, float SpreadAngle, const FVector& CentralDirection)
{
	if (NumDirections % 2)
	{
		OutDirections.Add(CentralDirection.GetSafeNormal());
	}

	if (NumDirections == 1)
	{
		return;
	}

	const int32 Half = NumDirections / 2;
	
	for (int32 Index = 1; Index <= Half; ++Index)
	{
		const float Angle = -Index * SpreadAngle * (NumDirections % 2 == 0 && Index == 1 ? 0.5f : 1.f);
		const FVector Rotated = CentralDirection.RotateAngleAxis(Angle, FVector::UpVector);
		OutDirections.Add(Rotated.GetSafeNormal());
	}
	
	for (int32 Index = 1; Index <= Half; ++Index)
	{
		const float Angle = Index * SpreadAngle * (NumDirections % 2 == 0 && Index == 1 ? 0.5f : 1.f);
		const FVector Rotated = CentralDirection.RotateAngleAxis(Angle, FVector::UpVector);
		OutDirections.Add(Rotated.GetSafeNormal());
	}
}
