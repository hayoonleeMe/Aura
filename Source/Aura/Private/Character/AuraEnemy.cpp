// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraEnemy.h"

#include "AuraBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Character/EnemyClassConfig.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/Widget/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	/* GAS */
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>(TEXT("Attribute Set"));

	/* Combat */
	Level = 1;

	/* Health Bar */
	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Bar Component"));
	HealthBarComponent->SetupAttachment(GetRootComponent());

	/* Dead */
	DeadLifeSpan = 5.f;
}

void AAuraEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraEnemy, bDead);
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

	// EnemyClassConfig에 정의된 데이터에 따라, EnemyClassType에 맞는 GameplayAbility 추가 
	if (HasAuthority())
	{
		if (const UEnemyClassConfig* EnemyClassConfig = UAuraBlueprintLibrary::GetEnemyClassConfig(this))
		{
			if (const FEnemyClassInfo* EnemyClassInfo = EnemyClassConfig->GetInfoByType(EnemyClassType))
			{
				AddStartupAbilities(EnemyClassInfo->StartupAbilities);
			}
			AddStartupAbilities(EnemyClassConfig->SharedAbilities);
		}
	}
}

void AAuraEnemy::InitializeAttributes()
{
	// EnemyClassConfig에 정의된 데이터에 따라, EnemyClassType에 맞는 GameplayEffect로 Attributes를 초기화
	if (HasAuthority())
	{
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

void AAuraEnemy::Die_Implementation()
{
	/* Called on server */
	// DeadLifeSpan 이후 Destroy
	SetLifeSpan(DeadLifeSpan);
	// Play dead animation and trigger rep notify
	bDead = true;
	HandleDeathLocally();
}

void AAuraEnemy::OnRep_Dead() const
{
	// Rep Notify로 Multicast RPC를 대신함
	HandleDeathLocally();
}

void AAuraEnemy::HandleDeathLocally() const
{
	// 충돌 방지
	FCollisionResponseContainer Container(ECR_Ignore);
	Container.SetResponse(ECC_WorldStatic, ECR_Block);
	Container.SetResponse(ECC_WorldDynamic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannels(Container);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// 로컬에서 죽었음을 알림 (Will hide enemy health bar)
	OnCharacterDeadDelegate.Broadcast();
}
