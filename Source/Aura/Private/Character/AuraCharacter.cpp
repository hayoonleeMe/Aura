// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Interface/StageSystemInterface.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/Widget/PlayerNameplate.h"
#include "GameFramework/GameModeBase.h"

AAuraCharacter::AAuraCharacter()
{
	// 리스폰 시 Enemy를 피해 항상 스폰되도록
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// SKM의 Animation과 Bone이 렌더링되지 않아도 업데이트되도록 설정
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	WeaponMeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	/* Role */
	RoleTag = AuraGameplayTags::Role_Player;
	
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

	if (IStageSystemInterface* StageSystemInterface = Cast<IStageSystemInterface>(GetWorld() ? GetWorld()->GetAuthGameMode() : nullptr))
	{
		StageSystemInterface->RequestPlayerRespawn(GetController<APlayerController>());
	}
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

void AAuraCharacter::ServerAddXPForTest_Implementation(int32 XPAmount)
{
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		AuraASC->ApplyXPGainEffect(XPAmount);
	}
}

void AAuraCharacter::ServerLevelUpForTest_Implementation()
{
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (const UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
		{
			const int32 NextLevel = AuraAS->GetLevel() + 1;
			const int32 XP = AuraAS->GetXP();
			const int32 XPRequirement = GetLevelUpXpRequirement(NextLevel);
			AuraASC->ApplyXPGainEffect(XPRequirement - XP);
		}
	}
}

void AAuraCharacter::ServerAddAttributePointForTest_Implementation()
{
	AddToAttributePoints(1);
}

void AAuraCharacter::ServerAddSpellPointForTest_Implementation()
{
	AddToSpellPoints(1);
}

void AAuraCharacter::BeginPlay()
{
	Super::BeginPlay();

	// PlayerName 표시
	InitializePlayerNameplateWidget();
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

	// Halo Of Protection Passive Ability Tag
	AuraASC->RegisterGameplayTagEvent(AuraGameplayTags::Abilities_Passive_HaloOfProtection).AddUObject(this, &ThisClass::OnHaloOfProtectionSiphonTagChanged);
	if (AuraASC->GetTagCount(AuraGameplayTags::Abilities_Passive_HaloOfProtection) > 0)
	{
		// 이벤트를 등록하기 전에 이미 패시브 어빌리티가 실행된 상태라면 호출해줌
		OnHaloOfProtectionSiphonTagChanged(AuraGameplayTags::Abilities_Passive_HaloOfProtection, AuraASC->GetTagCount(AuraGameplayTags::Abilities_Passive_HaloOfProtection));
	}

	// Health Siphon Passive Ability Tag
	AuraASC->RegisterGameplayTagEvent(AuraGameplayTags::Abilities_Passive_HealthSiphon).AddUObject(this, &ThisClass::OnHealthSiphonTagChanged);
	if (AuraASC->GetTagCount(AuraGameplayTags::Abilities_Passive_HealthSiphon) > 0)
	{
		// 이벤트를 등록하기 전에 이미 패시브 어빌리티가 실행된 상태라면 호출해줌
		OnHealthSiphonTagChanged(AuraGameplayTags::Abilities_Passive_HealthSiphon, AuraASC->GetTagCount(AuraGameplayTags::Abilities_Passive_HealthSiphon));
	}

	// Mana Siphon Passive Ability Tag
	AuraASC->RegisterGameplayTagEvent(AuraGameplayTags::Abilities_Passive_ManaSiphon).AddUObject(this, &ThisClass::OnManaSiphonTagChanged);
	if (AuraASC->GetTagCount(AuraGameplayTags::Abilities_Passive_ManaSiphon) > 0)
	{
		// 이벤트를 등록하기 전에 이미 패시브 어빌리티가 실행된 상태라면 호출해줌
		OnManaSiphonTagChanged(AuraGameplayTags::Abilities_Passive_ManaSiphon, AuraASC->GetTagCount(AuraGameplayTags::Abilities_Passive_ManaSiphon));
	}

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

		if (AAuraPlayerController* AuraPC = GetController<AAuraPlayerController>())
		{
			if (AuraPC->IsLocalController())
			{
				// Stage를 시작하기 위해 ASC 초기화를 ServerRPC로 전송
				AuraPC->ServerNotifyASCInitToGameMode();
			}
		}
	}
	else
	{
		/* Respawn */

		// 리스폰 무적 적용
		AuraASC->ApplyEffectSpecToSelfWithSetByCaller(InvincibilityEffectClass, AuraGameplayTags::Gameplay_Invincibility, RespawnInvincibilityTime);

		if (IsLocallyControlled())
		{
			if (AAuraPlayerController* AuraPC = GetController<AAuraPlayerController>())
			{
				AuraPC->EnableAbilityInput();
			}
		}
	}

	// 모든 Passive Ability 실행
	AuraASC->ActivateAllPassiveSpells();

	if (HasAuthority())
	{
		InitializeAttributes();
	}
	AuraASC->SetInitialized();

	// PlayerName 표시
	InitializePlayerNameplateWidget();

	// Audio Listener를 캐릭터로 설정
	if (APlayerController* PlayerController = GetController<APlayerController>())
	{
		PlayerController->SetAudioListenerOverride(GetRootComponent(), FVector::ZeroVector, FRotator::ZeroRotator);
	}
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

	if (IsLocallyControlled())
	{
		if (AAuraPlayerController* AuraPC = GetController<AAuraPlayerController>())
		{
			AuraPC->DisableAbilityInput();
		}
	}
}

void AAuraCharacter::OnHaloOfProtectionSiphonTagChanged(const FGameplayTag Tag, int32 NewCount) const
{
	if (NewCount > 0)
	{
		if (!HaloOfProtectionComponent->IsActive())
		{
			HaloOfProtectionComponent->Activate();
		}
	}
	else
	{
		HaloOfProtectionComponent->DeactivateImmediate();
	}
}

void AAuraCharacter::OnHealthSiphonTagChanged(const FGameplayTag Tag, int32 NewCount) const
{
	if (NewCount > 0)
	{
		if (!HealthSiphonComponent->IsActive())
		{
			HealthSiphonComponent->Activate();
		}
	}
	else
	{
		HealthSiphonComponent->DeactivateImmediate();
	}
}

void AAuraCharacter::OnManaSiphonTagChanged(const FGameplayTag Tag, int32 NewCount) const
{
	if (NewCount > 0)
	{
		if (!ManaSiphonComponent->IsActive())
		{
			ManaSiphonComponent->Activate();
		}
	}
	else
	{
		ManaSiphonComponent->DeactivateImmediate();
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
