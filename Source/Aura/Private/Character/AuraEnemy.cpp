// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraEnemy.h"

#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayCueManager.h"
#include "NiagaraComponent.h"
#include "AI/AuraAIController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Animation/AnimInstance.h"
#include "Aura/Aura.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/AudioComponent.h"
#include "Data/EnemyClassConfig.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/PlayerInterface.h"
#include "Materials/MaterialInstance.h"
#include "UI/Widget/EnemyHealthBar.h"

AAuraEnemy::AAuraEnemy()
{
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Enemy);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Enemy, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_Enemy);
	GetMesh()->SetCollisionResponseToChannel(ECC_Enemy, ECR_Ignore);
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Target, ECR_Block);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_STENCIL_RED);
	
	/* GAS */
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>(TEXT("Attribute Set"));
	
	/* Role */
	RoleTag = AuraGameplayTags::Role_Enemy;

	/* Combat */
	AttackEffectiveRange = 120.f;
	AttackRangeRadius = 10.f;
	AttackRangeHalfHeight = 30.f;

	/* Health Bar */
	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Bar Component"));
	HealthBarComponent->SetupAttachment(GetRootComponent());

	/* Dead */
	DeadLifeSpan = 5.f;

	/* Movement */
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 30.f;
	GetCharacterMovement()->AvoidanceWeight = 0.5f;

	/* Debuff Niagara Component */
	EnfeebleDebuffComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Enfeeble Debuff Component"));
	EnfeebleDebuffComponent->SetupAttachment(GetMesh(), TEXT("DebuffSocket"));
	EnfeebleDebuffComponent->bAutoActivate = false;
	
	IgniteDebuffComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Ignite Debuff Component"));
	IgniteDebuffComponent->SetupAttachment(GetMesh(), TEXT("DebuffSocket"));
	IgniteDebuffComponent->bAutoActivate = false;

	StunDebuffComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Stun Debuff Component"));
	StunDebuffComponent->SetupAttachment(GetRootComponent());
	StunDebuffComponent->bAutoActivate = false;
	
	StunSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Stun Sound Component"));
	StunSoundComponent->SetupAttachment(GetRootComponent());
	StunSoundComponent->bAutoActivate = false;
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// AI server only
	if (HasAuthority())
	{
		AuraAIController = Cast<AAuraAIController>(NewController);
		AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		AuraAIController->GetBlackboardComponent()->SetValueAsFloat(TEXT("AttackEffectiveRange"), AttackEffectiveRange);
		AuraAIController->GetBlackboardComponent()->SetValueAsFloat(TEXT("AttackAbortRange"), AttackAbortRange);
		AuraAIController->RunBehaviorTree(BehaviorTree);
	}
}

void AAuraEnemy::GetAttackCheckRange(float& OutRadius, float& OutHalfHeight) const
{
	OutRadius = AttackRangeRadius;
	OutHalfHeight = AttackRangeHalfHeight;
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
	InitializeEnemyLevel();
	InitializeAttributes();
	InitializeForHealthBar();

	// Execute SummonEffect GameplayCue
	FGameplayCueParameters CueParams;
	CueParams.Location = GetNavAgentLocation();
	UGameplayCueManager::ExecuteGameplayCue_NonReplicated(this, AuraGameplayTags::GameplayCue_SummonEffect, CueParams);
}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (HasAuthority())
	{
		// EnemyClassConfig에 정의된 데이터에 따라, EnemyClassType에 맞는 GameplayAbility 추가 
		if (const UEnemyClassConfig* EnemyClassConfig = UAuraBlueprintLibrary::GetEnemyClassConfig(this))
		{
			if (const FEnemyClassInfo* EnemyClassInfo = EnemyClassConfig->GetInfoByType(EnemyClassType))
			{
				AddStartupAbilities(EnemyClassInfo->StartupAbilities);
			}
			AddStartupAbilities(EnemyClassConfig->SharedAbilities);
		}
		// 특정 Enemy에 귀속되는 GameplayAbility 추가
		// - Enemy Spawn Level을 따름
		AddStartupAbilities(StartupAbilities, SpawnLevel);
	}

	// Abilities.HitReact Tag의 Added, Removed Event에 Binding
	AbilitySystemComponent->RegisterGameplayTagEvent(AuraGameplayTags::Abilities_HitReact).AddUObject(this, &ThisClass::OnHitReactTagChanged);

	// Debuff.Enfeeble Tag의 Added, Removed Event에 Binding
	AbilitySystemComponent->RegisterGameplayTagEvent(AuraGameplayTags::Debuff_Enfeeble).AddUObject(this, &ThisClass::OnDebuffEnfeebleTagChanged);
	
	// Debuff.Ignite Tag의 Added, Removed Event에 Binding
	AbilitySystemComponent->RegisterGameplayTagEvent(AuraGameplayTags::Debuff_Ignite).AddUObject(this, &ThisClass::OnDebuffIgniteTagChanged);

	// Debuff.Stun Tag의 Added, Removed Event에 Binding
	AbilitySystemComponent->RegisterGameplayTagEvent(AuraGameplayTags::Debuff_Stun).AddUObject(this, &ThisClass::OnDebuffStunTagChanged);
}

void AAuraEnemy::InitializeAttributes()
{
	// EnemyClassConfig에 정의된 데이터에 따라, EnemyClassType에 맞는 GameplayEffect로 Attributes를 초기화
	if (HasAuthority())
	{
		UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
		const int32 Level = UAuraBlueprintLibrary::GetLevelAttributeValue(this);
		if (const UEnemyClassConfig* EnemyClassConfig = UAuraBlueprintLibrary::GetEnemyClassConfig(this))
		{
			if (const FEnemyClassInfo* EnemyClassInfo = EnemyClassConfig->GetInfoByType(EnemyClassType))
			{
				AuraASC->ApplyEffectSpecToSelf(EnemyClassInfo->PrimaryAttributes, Level);
			}
			AuraASC->ApplyEffectSpecToSelf(EnemyClassConfig->SecondaryAttributes, Level);
			AuraASC->ApplyEffectSpecToSelf(EnemyClassConfig->VitalAttributes, Level);
		}
	}
}

void AAuraEnemy::OnHitReactTagChanged(const FGameplayTag Tag, int32 Count) const
{
	const bool bHitReact = Count > 0;
	if (bHitReact)
	{
		// HitReact - 움직임 방지
		GetCharacterMovement()->DisableMovement();
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	
	if (IsValid(AuraAIController) && AuraAIController->GetBlackboardComponent())
	{
		// AIController server only
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("HitReact"), bHitReact);
	}
}

void AAuraEnemy::InitializeEnemyLevel() const
{
	if (HasAuthority())
	{
		if (UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
		{
			AuraAS->InitLevel(SpawnLevel);
		}
	}
}

void AAuraEnemy::InitializeForHealthBar() const
{
	if (HealthBarComponent)
	{
		if (UEnemyHealthBar* HealthBarWidget = Cast<UEnemyHealthBar>(HealthBarComponent->GetWidget()))
		{
			HealthBarWidget->InitializeHealthBar(AbilitySystemComponent, AttributeSet);
		}
	}
}

void AAuraEnemy::Dissolve()
{
	if (UMaterialInstanceDynamic* MID = GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshDissolveMaterial))
	{
		StartDissolveTimeline(MID);
	}
	if (UMaterialInstanceDynamic* MID = WeaponMeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, WeaponMeshDissolveMaterial))
	{
		StartWeaponDissolveTimeline(MID);
	}
}

void AAuraEnemy::Die()
{
	Super::Die();
	
	// DeadLifeSpan 이후 Destroy
	SetLifeSpan(DeadLifeSpan);

	// Update Dead Blackboard Key
	if (IsValid(AuraAIController) && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("Dead"), bDead);
	}

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities();
	}
}

int32 AAuraEnemy::GetXPReward()
{
	checkf(XPRewardCurve.IsValid(), TEXT("Need to set XPRewardCurve"));
	const int32 XPReward = XPRewardCurve.GetValueAtLevel(UAuraBlueprintLibrary::GetLevelAttributeValue(this));
	return XPReward;
}

void AAuraEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
}

void AAuraEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
}

void AAuraEnemy::HandleDeathLocally()
{
	// 충돌 방지
	FCollisionResponseContainer Container(ECR_Ignore);
	Container.SetResponse(ECC_WorldStatic, ECR_Block);
	Container.SetResponse(ECC_WorldDynamic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannels(Container);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Enemy 죽음을 UI에 업데이트
	if (IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(GetWorld()->GetFirstPlayerController()))
	{
		PlayerInterface->NotifyEnemyDead();
	}
	
	// 로컬에서 죽었음을 알림 (Will hide enemy health bar)
	OnCharacterDeadDelegate.Broadcast();

	if (GetMesh() && GetMesh()->GetAnimInstance())
    {
    	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
    }

	Dissolve();

	// Deactivate Debuff Component
	EnfeebleDebuffComponent->DeactivateImmediate();
	IgniteDebuffComponent->DeactivateImmediate();
	StunDebuffComponent->DeactivateImmediate();
}

void AAuraEnemy::OnDebuffEnfeebleTagChanged(const FGameplayTag Tag, int32 Count) const
{
	// 모든 기기에서 호출되어 로컬에서 수행
	if (Count > 0)
	{
		if (!EnfeebleDebuffComponent->IsActive())
		{
			EnfeebleDebuffComponent->Activate();
		}
	}
	else
	{
		EnfeebleDebuffComponent->DeactivateImmediate();	
	}
}

void AAuraEnemy::OnDebuffIgniteTagChanged(const FGameplayTag Tag, int32 Count) const
{
	// 모든 기기에서 호출되어 로컬에서 수행
	if (Count > 0)
	{
		if (!IgniteDebuffComponent->IsActive())
		{
			IgniteDebuffComponent->Activate();
		}
	}
	else
	{
		IgniteDebuffComponent->DeactivateImmediate();	
	}
}

void AAuraEnemy::OnDebuffStunTagChanged(const FGameplayTag Tag, int32 Count) const
{
	// 모든 기기에서 호출되어 로컬에서 수행
	const bool bStun = Count > 0;
	if (bStun)
	{
		const FTaggedCombatInfo TaggedCombatInfo = GetTaggedCombatInfo(AuraGameplayTags::Debuff_Stun);
		check(TaggedCombatInfo.AnimMontage);

		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(TaggedCombatInfo.AnimMontage);
		}

		StunDebuffComponent->Activate();
		StunSoundComponent->Activate();
	}
	else
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Stop(0.2f);
		}

		StunDebuffComponent->DeactivateImmediate();
		StunSoundComponent->Deactivate();
	}

	if (IsValid(AuraAIController) && AuraAIController->GetBlackboardComponent())
	{
		// AIController server only
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stun"), bStun);
	}
}
