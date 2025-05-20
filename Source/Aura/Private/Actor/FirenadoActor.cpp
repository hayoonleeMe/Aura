// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/FirenadoActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "Aura/Aura.h"

AFirenadoActor::AFirenadoActor()
{
	SetReplicates(false);
	SetActorEnableCollision(false);
	SetCanBeDamaged(false);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Default Root Component"));
	
	RepeatActionDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::RepeatAction);
}

void AFirenadoActor::Initialize(float InEffectiveRadius, float InExplosionEffectiveRadius, float InScaleRate, float InDamagePerInterval, float InFinalDamageRate,
	float InDamageInterval, int32 InTotalDamageCount, const FVector& InTargetLocation, const FDamageEffectParams& InDamageEffectParams,
	UAbilitySystemComponent* InSourceASC)
{
	EffectiveRadius = InEffectiveRadius;
	ExplosionEffectiveRadius = InExplosionEffectiveRadius;
	ScaleRate = InScaleRate;
	DamagePerInterval = InDamagePerInterval;
	FinalDamageRate = InFinalDamageRate;
	DamageInterval = InDamageInterval;
	TotalActionCount = InTotalDamageCount;
	TargetLocation = InTargetLocation;
	DamageEffectParams = InDamageEffectParams;
	SourceASC = InSourceASC;
}

void AFirenadoActor::BeginPlay()
{
	Super::BeginPlay();

	FGameplayCueParameters CueParameters;
	CueParameters.RawMagnitude = FMath::RoundToFloat(DamageInterval * TotalActionCount);
	CueParameters.NormalizedMagnitude = ScaleRate;
	CueParameters.Location = TargetLocation;
	CueParameters.TargetAttachComponent = RootComponent;
	SourceASC->ExecuteGameplayCue(AuraGameplayTags::GameplayCue_Firenado, CueParameters);

	if (GetWorld())
	{
		// 반복을 위한 Timer 등록
		GetWorld()->GetTimerManager().SetTimer(RepeatActionTimerHandle, RepeatActionDelegate, DamageInterval, true, 0);
	}
}

void AFirenadoActor::RepeatAction()
{
	++ActiveActionIndex;
	if (ActiveActionIndex <= TotalActionCount)
	{
		ApplyDamagePerInterval();
	}
	else
	{
		SpawnFinalExplosion();

		// 마지막 데미지까지 입히고 타이머를 정리하고 제거한다.
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(RepeatActionTimerHandle);
		}
		Destroy();
	}
}

void AFirenadoActor::ApplyDamagePerInterval()
{
	const float FinalEffectiveRadius = EffectiveRadius * ScaleRate;
	
	TArray<AActor*> EnemiesToApplyDamage;
	UAuraBlueprintLibrary::GetEnemiesOverlappedByChannel(GetWorld(), EnemiesToApplyDamage, TargetLocation, FQuat::Identity, ECC_Target, FCollisionShape::MakeSphere(FinalEffectiveRadius));

	// Apply Damage Effect
	for (AActor* Enemy : EnemiesToApplyDamage)
	{
		DamageEffectParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Enemy);
		UAuraBlueprintLibrary::ApplyDamageEffect(DamageEffectParams);
	}
}

void AFirenadoActor::SpawnFinalExplosion()
{
	const float FinalExplosionEffectiveRadius = ExplosionEffectiveRadius * ScaleRate;

	FGameplayCueParameters CueParameters;
	CueParameters.RawMagnitude = ScaleRate;
	CueParameters.Location = TargetLocation + FVector(0.f, 0.f, FinalExplosionEffectiveRadius * 0.5f);
	SourceASC->ExecuteGameplayCue(AuraGameplayTags::GameplayCue_Firenado_Explosion, CueParameters);

	TArray<AActor*> EnemiesToApplyDamage;
	UAuraBlueprintLibrary::GetEnemiesOverlappedByChannel(GetWorld(), EnemiesToApplyDamage, TargetLocation, FQuat::Identity, ECC_Target, FCollisionShape::MakeSphere(FinalExplosionEffectiveRadius));
	
	const float FinalDamage = DamagePerInterval * FinalDamageRate;

	// Apply Damage Effect
	for (AActor* Enemy : EnemiesToApplyDamage)
	{
		DamageEffectParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Enemy);
		DamageEffectParams.BaseDamage = FinalDamage;
		UAuraBlueprintLibrary::ApplyDamageEffect(DamageEffectParams);
	}
}

