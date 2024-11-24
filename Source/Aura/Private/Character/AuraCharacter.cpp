// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// for server (PossessedBy is called on server)
	InitAbilityActorInfo();
	InitializeAttributes();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// for client
	InitAbilityActorInfo();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPS = GetPlayerStateChecked<AAuraPlayerState>();
	AbilitySystemComponent = AuraPS->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(AuraPS, this);
	AttributeSet = AuraPS->GetAttributeSet();

	AddStartupAbilities();
}

void AAuraCharacter::InitializeAttributes()
{
	check(GetAbilitySystemComponent());

	for (const TSubclassOf<UGameplayEffect>& EffectClass : DefaultEffects)
	{
		if (EffectClass)
		{
			FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
			ContextHandle.AddSourceObject(this);
			FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(EffectClass, 1.f, ContextHandle);
			GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
	}
}
