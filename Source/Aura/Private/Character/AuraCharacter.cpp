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

int32 AAuraCharacter::GetCharacterLevel()
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

	for (const TSubclassOf<UGameplayEffect>& EffectClass : DefaultEffects)
	{
		ApplyEffectSpecToSelf(EffectClass);
	}
}

void AAuraCharacter::HandleDeathLocally() const
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
