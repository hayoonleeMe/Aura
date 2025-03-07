// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Component/ObjectPoolComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Game/StageGameMode.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/PlayerNameplate.h"

AAuraCharacter::AAuraCharacter()
{
	// 리스폰 시 Enemy를 피해 항상 스폰되도록
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	/* Role */
	RoleTag = AuraGameplayTags::Role_Player;
	
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

	/* Invincibility */
	RespawnInvincibilityTime = 3.f;

	/* Player Nameplate */
	PlayerNameplateWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Player Nameplate Widget Component"));
	PlayerNameplateWidgetComponent->SetupAttachment(GetRootComponent());
	PlayerNameplateWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	PlayerNameplateWidgetComponent->SetDrawAtDesiredSize(true);
	PlayerNameplateWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));

	/* Obstacle Check */
	ObstacleCheckBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Obstacle Check Box"));
	ObstacleCheckBox->SetupAttachment(SpringArmComponent);
	ObstacleCheckBox->SetBoxExtent(FVector(350.f, 600.f, 140.f));
	ObstacleCheckBox->SetRelativeLocation(FVector(630.f, 0.f, -50.f));
	ObstacleCheckBox->SetCollisionObjectType(ECC_OnlyWall);
	ObstacleCheckBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ObstacleCheckBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	ObstacleCheckBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	ObstacleCheckBox->SetGenerateOverlapEvents(true);
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

void AAuraCharacter::InitializePlayerNameplateWidget() const
{
	if (PlayerNameplateWidgetComponent && GetPlayerState())
	{
		if (const UPlayerNameplate* PlayerNameplateWidget = Cast<UPlayerNameplate>(PlayerNameplateWidgetComponent->GetWidget()))
		{
			PlayerNameplateWidget->SetPlayerName(GetPlayerState()->GetPlayerName());
		}
	}
}

AActor* AAuraCharacter::SpawnFromPool(EPooledActorType PooledActorType, const FTransform& SpawnTransform)
{
	if (PooledActorType == EPooledActorType::FireBolt)
	{
		return FireBoltPoolComponent->SpawnFromPool(SpawnTransform);
	}
	return nullptr;
}

void AAuraCharacter::MulticastActivatePassiveSpellNiagaraComponent_Implementation(const FGameplayTag& SpellTag) const
{
	if (SpellTag.MatchesTagExact(AuraGameplayTags::Abilities_Passive_HaloOfProtection))
	{
		HaloOfProtectionComponent->Activate();
	}
	else if (SpellTag.MatchesTagExact(AuraGameplayTags::Abilities_Passive_HealthSiphon))
	{
		HealthSiphonComponent->Activate();
	}
	else if (SpellTag.MatchesTagExact(AuraGameplayTags::Abilities_Passive_ManaSiphon))
	{
		ManaSiphonComponent->Activate();
	}
}

void AAuraCharacter::MulticastDeactivatePassiveSpellNiagaraComponent_Implementation(const FGameplayTag& SpellTag) const
{
	if (SpellTag.MatchesTagExact(AuraGameplayTags::Abilities_Passive_HaloOfProtection))
	{
		HaloOfProtectionComponent->DeactivateImmediate();
	}
	else if (SpellTag.MatchesTagExact(AuraGameplayTags::Abilities_Passive_HealthSiphon))
	{
		HealthSiphonComponent->DeactivateImmediate();
	}
	else if (SpellTag.MatchesTagExact(AuraGameplayTags::Abilities_Passive_ManaSiphon))
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

	// 무적 Effect Material 표시를 위해 Invincibility Tag Changed Delegate에 콜백 함수 등록
	AuraASC->RegisterGameplayTagEvent(AuraGameplayTags::Gameplay_Invincibility).AddUObject(this, &ThisClass::OnInvincibilityTagChanged);

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
		if (const APlayerController* PC = GetController<APlayerController>())
		{
			if (AAuraHUD* AuraHUD = PC->GetHUD<AAuraHUD>())
			{
				AuraHUD->InitOverlay();
			}
		}
	}
	else
	{
		/* Respawn */
		
		// 모든 Passive Ability 실행
		AuraASC->ActivateAllPassiveSpells();

		// 리스폰 무적 적용
		AuraASC->ApplyEffectSpecToSelfWithSetByCaller(InvincibilityEffectClass, AuraGameplayTags::Gameplay_Invincibility, RespawnInvincibilityTime);

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

	// 캐릭터 스폰 시 생명 주기 함수 호출이 어긋나므로 다음 프레임에 호출해 해결
	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::InitializePlayerNameplateWidget));
}

void AAuraCharacter::InitializeAttributes()
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
	const bool bASCAlreadyInitialized = AuraASC->IsInitialized();
	
	for (const TSubclassOf<UGameplayEffect>& EffectClass : StartupEffects)
	{
		if (bASCAlreadyInitialized)
		{
			// 리스폰 시에는 EffectRules.PerLife Tag를 가지는 StartupEffect만 적용
			if (EffectClass->GetDefaultObject<UGameplayEffect>()->InheritableGameplayEffectTags.CombinedTags.HasTagExact(AuraGameplayTags::EffectRules_PerLife))
			{
				AuraASC->ApplyEffectSpecToSelf(EffectClass);
			}
		}
		else
		{
			// 첫 스폰 시에는 모든 StartupEffect 적용
			AuraASC->ApplyEffectSpecToSelf(EffectClass);
		}
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

		// 캐릭터가 죽을 때 EffectRules.PerLife Tag를 가지는 이펙트만 제거
		FGameplayTagContainer QueryTags;
		QueryTags.AddTag(AuraGameplayTags::EffectRules_PerLife);
		AbilitySystemComponent->RemoveActiveEffectsWithTags(QueryTags);
	}
}

void AAuraCharacter::OnInvincibilityTagChanged(const FGameplayTag Tag, int32 NewCount) const
{
	if (NewCount > 0)
	{
		// 무적을 나타내는 Material 표시
		GetMesh()->SetOverlayMaterial(InvincibilityEffectMaterial);
		WeaponMeshComponent->SetOverlayMaterial(InvincibilityEffectMaterial);
	}
	else
	{
		// 무적을 나타내는 Material 제거
		GetMesh()->SetOverlayMaterial(nullptr);
		WeaponMeshComponent->SetOverlayMaterial(nullptr);
	}
}
