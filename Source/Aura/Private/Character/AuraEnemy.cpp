// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraEnemy.h"

#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AI/AuraAIController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Data/EnemyClassConfig.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Target, ECR_Block);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_STENCIL_RED);
	
	/* GAS */
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>(TEXT("Attribute Set"));

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
	InitializeAttributes();
	InitializeForHealthBar();
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
		AddStartupAbilities(StartupAbilities);
	}

	// Abilities.HitReact Tag의 Added, Removed Event에 Binding
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Abilities_HitReact).AddUObject(this, &ThisClass::OnHitReactTagChanged);
}

void AAuraEnemy::InitializeAttributes()
{
	// EnemyClassConfig에 정의된 데이터에 따라, EnemyClassType에 맞는 GameplayEffect로 Attributes를 초기화
	if (HasAuthority())
	{
		const int32 Level = UAuraBlueprintLibrary::GetLevelAttributeValue(this);
		if (const UEnemyClassConfig* EnemyClassConfig = UAuraBlueprintLibrary::GetEnemyClassConfig(this))
		{
			if (const FEnemyClassInfo* EnemyClassInfo = EnemyClassConfig->GetInfoByType(EnemyClassType))
			{
				ApplyEffectSpecToSelf(EnemyClassInfo->PrimaryAttributes, Level);
			}
			ApplyEffectSpecToSelf(EnemyClassConfig->SecondaryAttributes, Level);
			ApplyEffectSpecToSelf(EnemyClassConfig->VitalAttributes, Level);
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

void AAuraEnemy::InitializeForHealthBar()
{
	check(AbilitySystemComponent && AttributeSet && HealthBarComponent);

	// AuraEnemy가 Widget Controller 역할을 함
	if (UAuraUserWidget* HealthBarWidget = Cast<UAuraUserWidget>(HealthBarComponent->GetWidget()))
	{
		HealthBarWidget->SetWidgetController(this);
	}

	if (const UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		// Attribute Changed Event를 알리기 위해 바인딩
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnHealthChangedDelegate.Broadcast(Data.NewValue);
		});
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChangedDelegate.Broadcast(Data.NewValue);
		});

		// 초기값 Broadcast
		OnHealthChangedDelegate.Broadcast(AuraAS->GetHealth());
		OnMaxHealthChangedDelegate.Broadcast(AuraAS->GetMaxHealth());
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
	
	// 로컬에서 죽었음을 알림 (Will hide enemy health bar)
	OnCharacterDeadDelegate.Broadcast();

	if (GetMesh() && GetMesh()->GetAnimInstance())
    {
    	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
    }

	Dissolve();
}
