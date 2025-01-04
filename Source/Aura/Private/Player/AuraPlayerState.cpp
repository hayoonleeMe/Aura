// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	// PlayerState의 기본 NetUpdateFrequency는 낮기 때문에 PlayerState에서 ASC와 AS를 가지기 위해선 높여줘야 한다.
	NetUpdateFrequency = 100.f;
	
	/* GAS */
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>(TEXT("Attribute Set"));

	/* Combat */
	AttributePoints = 0;
	SpellPoints = 0;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

void AAuraPlayerState::OnRep_AttributePoints() const
{
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::AddToAttributePoints(int32 InValue)
{
	AttributePoints += InValue;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints() const
{
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::AddToSpellPoints(int32 InValue)
{
	SpellPoints += InValue;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

int32 AAuraPlayerState::GetLevelUpXpRequirement(int32 Level) const
{
	const int32 RetValue = XPRequirementCurve.GetValueAtLevel(Level); 
	return RetValue;
}

int32 AAuraPlayerState::GetLevelUpAttributePointsAward(int32 Level) const
{
	const int32 RetValue = AttributePointsAwardCurve.GetValueAtLevel(Level); 
	return RetValue;
}

int32 AAuraPlayerState::GetLevelUpSpellPointsAward(int32 Level) const
{
	const int32 RetValue = SpellPointsAwardCurve.GetValueAtLevel(Level); 
	return RetValue;
}
