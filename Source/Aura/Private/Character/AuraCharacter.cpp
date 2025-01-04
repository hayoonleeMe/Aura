// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

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

	/* Movement */
	GetCharacterMovement()->bOrientRotationToMovement = true;   // 캐릭터 가속 방향으로 회전시킴
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;         // 이동을 평면에 제한
	GetCharacterMovement()->bSnapToPlaneAtStart = true;       // 시작 시 캐릭터가 평면에 부착됨
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

int32 AAuraCharacter::GetAttributePoints() const
{
	const AAuraPlayerState* AuraPS = GetPlayerStateChecked<AAuraPlayerState>();
	return AuraPS->GetAttributePoints();
}

void AAuraCharacter::AddToAttributePoints(int32 Points)
{
	AAuraPlayerState* AuraPS = GetPlayerStateChecked<AAuraPlayerState>();
	AuraPS->AddToAttributePoints(Points);
}

void AAuraCharacter::IncrementAttributePoints()
{
	AAuraPlayerState* AuraPS = GetPlayerStateChecked<AAuraPlayerState>();
	AuraPS->IncrementAttributePoints();
}

void AAuraCharacter::DecrementAttributePoints()
{
	AAuraPlayerState* AuraPS = GetPlayerStateChecked<AAuraPlayerState>();
	AuraPS->DecrementAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints() const
{
	const AAuraPlayerState* AuraPS = GetPlayerStateChecked<AAuraPlayerState>();
	return AuraPS->GetSpellPoints();
}

void AAuraCharacter::AddToSpellPoints(int32 Points)
{
	AAuraPlayerState* AuraPS = GetPlayerStateChecked<AAuraPlayerState>();
	AuraPS->AddToSpellPoints(Points);
}

void AAuraCharacter::IncrementSpellPoints()
{
	AAuraPlayerState* AuraPS = GetPlayerStateChecked<AAuraPlayerState>();
	AuraPS->IncrementSpellPoints();
}

void AAuraCharacter::DecrementSpellPoints()
{
	AAuraPlayerState* AuraPS = GetPlayerStateChecked<AAuraPlayerState>();
	AuraPS->DecrementSpellPoints();
}

void AAuraCharacter::IndicateDamage(float Damage, bool bIsBlockedHit, bool bIsCriticalHit, const FVector_NetQuantize& TargetLocation) const
{
	if (const AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPC->ClientIndicateDamage(Damage, bIsBlockedHit, bIsCriticalHit, TargetLocation);
	}
}

int32 AAuraCharacter::GetLevelUpXpRequirement(int32 Level) const
{
	const AAuraPlayerState* AuraPS = GetPlayerStateChecked<AAuraPlayerState>();
	return AuraPS->GetLevelUpXPRequirement(Level);
}

int32 AAuraCharacter::GetLevelUpAttributePointsAward(int32 Level) const
{
	const AAuraPlayerState* AuraPS = GetPlayerStateChecked<AAuraPlayerState>();
	return AuraPS->GetLevelUpAttributePointsAward(Level);
}

int32 AAuraCharacter::GetLevelUpSpellPointsAward(int32 Level) const
{
	const AAuraPlayerState* AuraPS = GetPlayerStateChecked<AAuraPlayerState>();
	return AuraPS->GetLevelUpSpellPointsAward(Level);
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPS = GetPlayerStateChecked<AAuraPlayerState>();
	AbilitySystemComponent = AuraPS->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(AuraPS, this);
	AttributeSet = AuraPS->GetAttributeSet();

	// Overlay Widget 생성 전에 Startup Ability 추가
	AddStartupAbilities(StartupAbilities);
	
	// Overlay Widget 초기화
	if (AAuraPlayerController* AuraPC = GetController<AAuraPlayerController>())
	{
		if (AAuraHUD* AuraHUD = AuraPC->GetHUD<AAuraHUD>())
		{
			AuraHUD->InitOverlay(AuraPC, AuraPS, AbilitySystemComponent, AttributeSet);
		}
	}
}

void AAuraCharacter::InitializeAttributes()
{
	check(GetAbilitySystemComponent());

	for (const TSubclassOf<UGameplayEffect>& EffectClass : StartupEffects)
	{
		ApplyEffectSpecToSelf(EffectClass);
	}
}

void AAuraCharacter::HandleDeathLocally()
{
	// 충돌 방지
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FCollisionResponseContainer Container(ECR_Ignore);
	Container.SetResponse(ECC_WorldStatic, ECR_Block);
	Container.SetResponse(ECC_WorldDynamic, ECR_Block);
	GetMesh()->SetCollisionResponseToChannels(Container);
	WeaponMeshComponent->SetCollisionResponseToChannels(Container);
	
	// Ragdoll
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	WeaponMeshComponent->SetSimulatePhysics(true);		
	WeaponMeshComponent->SetEnableGravity(true);
}
