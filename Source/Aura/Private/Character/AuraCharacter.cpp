// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Actor/FireBolt_Pooled.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Component/ObjectPoolComponent.h"
#include "Game/StageGameMode.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	// 리스폰 시 Enemy를 피해 항상 스폰되도록
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; 
	
	/* Projectile Pool */
	FireBoltPoolComponent = CreateDefaultSubobject<UObjectPoolComponent>(TEXT("Projectile Pool Component"));
	FireBoltPoolComponent->SetPoolSize(100);
	FireBoltPoolComponent->bAutoActivate = true;
	
	/* Camera */
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->SetWorldRotation(FRotator(-55.f, 0.f, 0.f));
	SpringArmComponent->TargetArmLength = 1000.f;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	/* Movement */
	GetCharacterMovement()->bOrientRotationToMovement = true;   // 캐릭터 가속 방향으로 회전시킴
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;         // 이동을 평면에 제한
	GetCharacterMovement()->bSnapToPlaneAtStart = true;       // 시작 시 캐릭터가 평면에 부착됨

	/* Passive Niagara Effect Component */
	HaloOfProtectionComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Halo Of Protection Component"));
	HaloOfProtectionComponent->SetupAttachment(GetRootComponent());
	HaloOfProtectionComponent->bAutoActivate = false;

	HealthSiphonComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Health Siphon Component"));
	HealthSiphonComponent->SetupAttachment(GetRootComponent());
	HealthSiphonComponent->bAutoActivate = false;
	
	ManaSiphonComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Mana Siphon Component"));
	ManaSiphonComponent->SetupAttachment(GetRootComponent());
	ManaSiphonComponent->bAutoActivate = false;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// for server (PossessedBy is called on server)
	InitAbilityActorInfo();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// for client
	InitAbilityActorInfo();
}

void AAuraCharacter::Die()
{
	Super::Die();

	if (AStageGameMode* StageGameMode = GetWorld()->GetAuthGameMode<AStageGameMode>())
	{
		StageGameMode->RequestPlayerRespawn(GetController<APlayerController>());
	}
}

void AAuraCharacter::OnPassiveSpellActivated(const FGameplayTag& SpellTag) const
{
	MulticastActivatePassiveSpellNiagaraComponent(SpellTag);
}

void AAuraCharacter::OnPassiveSpellDeactivated(const FGameplayTag& SpellTag) const
{
	MulticastDeactivatePassiveSpellNiagaraComponent(SpellTag);
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

AActor* AAuraCharacter::SpawnFromPool(const TSubclassOf<AActor>& Class, const FTransform& SpawnTransform)
{
	if (Class->IsChildOf(AFireBolt_Pooled::StaticClass()))
	{
		return FireBoltPoolComponent->SpawnFromPool(SpawnTransform);
	}
	return nullptr;
}

void AAuraCharacter::MulticastActivatePassiveSpellNiagaraComponent_Implementation(const FGameplayTag& SpellTag) const
{
	if (SpellTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Passive_HaloOfProtection))
	{
		HaloOfProtectionComponent->Activate();
	}
	else if (SpellTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Passive_HealthSiphon))
	{
		HealthSiphonComponent->Activate();
	}
	else if (SpellTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Passive_ManaSiphon))
	{
		ManaSiphonComponent->Activate();
	}
}

void AAuraCharacter::MulticastDeactivatePassiveSpellNiagaraComponent_Implementation(const FGameplayTag& SpellTag) const
{
	if (SpellTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Passive_HaloOfProtection))
	{
		HaloOfProtectionComponent->DeactivateImmediate();
	}
	else if (SpellTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Passive_HealthSiphon))
	{
		HealthSiphonComponent->DeactivateImmediate();
	}
	else if (SpellTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Passive_ManaSiphon))
	{
		ManaSiphonComponent->DeactivateImmediate();
	}
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPS = GetPlayerStateChecked<AAuraPlayerState>();
	AbilitySystemComponent = AuraPS->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(AuraPS, this);
	AttributeSet = AuraPS->GetAttributeSet();

	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);

	// 가장 처음 게임이 시작했을 때
	if (!AuraASC->IsInitialized())
	{
		// Overlay Widget 생성 전에 Startup Ability 추가
		AddStartupAbilities(StartupAbilities);

		if (!AuraASC->AbilityFailedCallbacks.IsBoundToObject(AuraASC))
		{
			// 어빌리티 실행에 실패할 때 콜백 함수 등록
			AuraASC->AbilityFailedCallbacks.AddUObject(AuraASC, &UAuraAbilitySystemComponent::OnAbilityFailed);
		}

		// Overlay Widget 초기화
		if (AAuraPlayerController* AuraPC = GetController<AAuraPlayerController>())
		{
			if (AAuraHUD* AuraHUD = AuraPC->GetHUD<AAuraHUD>())
			{
				AuraHUD->InitOverlay(AuraPC, AuraPS, AbilitySystemComponent, AttributeSet);
			}
		}
	}
	else
	{
		/* Respawn */
		
		// 모든 Passive Ability 실행
		AuraASC->ActivateAllPassiveSpells();

		if (IsLocallyControlled())
		{
			if (APlayerController* PC = GetController<APlayerController>())
			{
				// Enable Input
				PC->EnableInput(PC);
			}
		}
	}

	if (HasAuthority())
	{
		InitializeAttributes();
	}
	AuraASC->SetInitialized();
}

void AAuraCharacter::InitializeAttributes()
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
	const bool bASCAlreadyInitialized = AuraASC->IsInitialized();
	const FGameplayTag PrimaryAttributesTag = FAuraGameplayTags::Get().Attributes_Primary;
	
	for (const TSubclassOf<UGameplayEffect>& EffectClass : StartupEffects)
	{
		if (bASCAlreadyInitialized)
		{
			if (const UGameplayEffect* Effect = EffectClass ? EffectClass->GetDefaultObject<UGameplayEffect>() : nullptr)
			{
				// 리스폰 시 Primary Attributes를 초기화하는 Effect 적용 방지
				if (Effect->InheritableGameplayEffectTags.CombinedTags.HasTag(PrimaryAttributesTag))
				{
					continue;
				}
			}
		}
		
		AuraASC->ApplyEffectSpecToSelf(EffectClass);
	}
}

void AAuraCharacter::HandleDeathLocally()
{
	// Disable Input
	if (APlayerController* PC = GetController<APlayerController>())
	{
		PC->DisableInput(PC);
	}

	// Disable Movement
	if (UCharacterMovementComponent* MoveComponent = GetCharacterMovement())
	{
		MoveComponent->StopMovementImmediately();
		MoveComponent->DisableMovement();
	}
	
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

	if (AbilitySystemComponent)
	{
		// Cancel All Abilities
		AbilitySystemComponent->CancelAbilities();

		// Remove All Active Effects
		TArray<FActiveGameplayEffectHandle> ActiveEffectHandles = AbilitySystemComponent->GetActiveGameplayEffects().GetAllActiveEffectHandles();
		for (const FActiveGameplayEffectHandle& EffectHandle : ActiveEffectHandles)
		{
			AbilitySystemComponent->RemoveActiveGameplayEffect(EffectHandle);
		}
	}
}
