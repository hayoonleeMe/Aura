// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/LightningBindActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectTypes.h"
#include "Aura/Aura.h"

ALightningBindActor::ALightningBindActor()
{
	SetReplicates(false);
	SetActorEnableCollision(false);
	SetCanBeDamaged(false);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Default Root Component"));
	
	RepeatActionDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::RepeatAction);
}

void ALightningBindActor::Initialize(float InFinalEffectiveRadius, float InScaleRate, float InDamageInterval, int32 InTotalDamageCount, const FVector& InTargetLocation,
	const FDamageEffectParams& InDamageEffectParams, UAbilitySystemComponent* InSourceASC)
{
	FinalEffectiveRadius = InFinalEffectiveRadius;
	ScaleRate = InScaleRate;
	DamageInterval = InDamageInterval;
	TotalActionCount = InTotalDamageCount;
	TargetLocation = InTargetLocation;
	DamageEffectParams = InDamageEffectParams;
	SourceASC = InSourceASC;
}

void ALightningBindActor::BeginPlay()
{
	Super::BeginPlay();

	if (SourceASC)
	{
		FGameplayCueParameters CueParams;
		CueParams.RawMagnitude = DamageInterval * TotalActionCount;
		CueParams.NormalizedMagnitude = ScaleRate;
		CueParams.Location = TargetLocation;
		CueParams.TargetAttachComponent = RootComponent;
		SourceASC->ExecuteGameplayCue(AuraGameplayTags::GameplayCue_LightningBind, CueParams);
	}
	
	if (GetWorld())
	{
		// 반복을 위한 Timer 등록
		GetWorld()->GetTimerManager().SetTimer(RepeatActionTimerHandle, RepeatActionDelegate, DamageInterval, true, 0);
	}
}

void ALightningBindActor::RepeatAction()
{
	++ActiveActionIndex;
	if (ActiveActionIndex <= TotalActionCount)
	{
		ApplyDamagePerInterval();
	}
	else
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(RepeatActionTimerHandle);
		}
		Destroy();
	}
}

void ALightningBindActor::ApplyDamagePerInterval()
{
	TArray<AActor*> EnemiesToApplyDamage;
	UAuraBlueprintLibrary::GetEnemiesOverlappedByChannel(GetWorld(), EnemiesToApplyDamage, TargetLocation, FQuat::Identity, ECC_Target, FCollisionShape::MakeSphere(FinalEffectiveRadius));

	// Apply Damage Effect
	// Stun Debuff Chance, Duration은 UAuraAbility_LightningBind에서 설정한다.
	for (AActor* Enemy : EnemiesToApplyDamage)
	{
		DamageEffectParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Enemy);
		UAuraBlueprintLibrary::ApplyDamageEffect(DamageEffectParams);
	}
}
