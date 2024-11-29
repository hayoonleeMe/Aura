// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraEnemy.h"

#include "AuraBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Character/EnemyClassConfig.h"

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
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
	InitializeAttributes();
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
