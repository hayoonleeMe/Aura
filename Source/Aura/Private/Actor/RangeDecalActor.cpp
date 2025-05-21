// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/RangeDecalActor.h"

#include "AbilitySystem/Abilities/AuraAbility_Firenado.h"
#include "Components/DecalComponent.h"
#include "Player/AuraPlayerController.h"

ARangeDecalActor::ARangeDecalActor()
{
	SetReplicates(false);
	SetActorEnableCollision(false);
	SetCanBeDamaged(false);

	// Tick을 사용할 수 있게 설정하고 처음엔 비활성화
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Default Root Component"));
	
	RangeDecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("Range Decal Component"));
	RangeDecalComponent->SetupAttachment(RootComponent);
	RangeDecalComponent->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	RangeDecalComponent->DecalSize = FVector(1.f, 0.f, 0.f);
}

void ARangeDecalActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ShowRangeDecal(true) -> SetActorTickEnabled(true) 에 의해 Tick이 활성화될 때만 Tick이 호출된다.
	if (OwningAbility && AuraPC)
	{
		if (const FHitResult HitResult = AuraPC->GetTargetHitResult(); HitResult.bBlockingHit)
		{
			// 보정이 끝난 위치에 액터를 표시해 최대 사거리르 넘어가지 않도록 한다.
			SetActorLocation(OwningAbility->ComputeValidTargetLocation(HitResult.ImpactPoint));
		}
	}
}

void ARangeDecalActor::Initialize(UAuraAbility_CastConfirmBase* InOwningAbility)
{
	OwningAbility = InOwningAbility;
	if (OwningAbility)
	{
		AuraPC = Cast<AAuraPlayerController>(OwningAbility->GetCurrentActorInfo()->PlayerController.Get()); 
	}
}

void ARangeDecalActor::ShowRangeDecal(bool bShow, float Radius)
{
	if (bShow)
	{
		SetActorHiddenInGame(false);
		SetActorTickEnabled(true);
		// Firenado 범위만큼 표시
		RangeDecalComponent->DecalSize.Y = RangeDecalComponent->DecalSize.Z = Radius;
	}
	else
	{
		SetActorHiddenInGame(true);
		SetActorTickEnabled(false);
	}
}
