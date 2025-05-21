// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/DamageEffectParams.h"
#include "FirenadoActor.generated.h"

/**
 * Audio-Visual Effect를 적용하고, 실제로 데미지를 입힌다. 
 * UAuraAbility_Firenado 어빌리티에 의해 생성된다.
 * Replication, Collision, Tick 등을 제외한 최소한의 기능만 수행한다.
 */
UCLASS()
class AURA_API AFirenadoActor : public AActor
{
	GENERATED_BODY()

public:
	AFirenadoActor();

	// 필요한 모든 데이터를 UAuraAbility_Firenado에서 주입받는다.
	void Initialize(float InFinalEffectiveRadius, float InFinalExplosionEffectiveRadius, float InScaleRate, float InDamagePerInterval, float InFinalDamageRate,  float InDamageInterval, int32 InTotalDamageCount, const FVector& InTargetLocation, const FDamageEffectParams& InDamageEffectParams, UAbilitySystemComponent* InSourceASC);

protected:
	virtual void BeginPlay() override;

private:
	/**
	 * UAuraAbility_Firenado의 데이터를 Initialize() 함수로 주입받아 사용
	 * @see UAuraAbility_Firenado
	 * @see Initialize()
	 */
	
	float FinalEffectiveRadius;
	float FinalExplosionEffectiveRadius;
	float ScaleRate;
	float DamagePerInterval;
	float FinalDamageRate;
	float DamageInterval;
	FVector TargetLocation;
	FDamageEffectParams DamageEffectParams;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;

	// 일정 범위 내의 적에게 반복적으로 데미지를 입기 위해 사용하는 타이머
	FTimerHandle RepeatActionTimerHandle;

	void RepeatAction();
	FTimerDelegate RepeatActionDelegate;

	// 실제 데미지를 가한 횟수
	int32 ActiveActionIndex = 0;

	// 데미지를 가할 총 횟수
	int32 TotalActionCount = 0;

	// 일정 간격으로 입히는 데미지를 가한다.
	void ApplyDamagePerInterval();

	// 마지막 폭발로 추가로 데미지를 입힌다.
	void SpawnFinalExplosion();
};
