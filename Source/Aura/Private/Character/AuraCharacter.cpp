// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/AuraPlayerState.h"

AAuraCharacter::AAuraCharacter()
{
	/* Camera */
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->SetWorldRotation(FRotator(-55.f, 0.f, 0.f));
	SpringArmComponent->TargetArmLength = 800.f;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
}

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

int32 AAuraCharacter::GetCharacterLevel_Implementation()
{
	const AAuraPlayerState* AuraPS = GetPlayerState<AAuraPlayerState>();
	check(AuraPS);
	return AuraPS->GetCharacterLevel();
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
