// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/DamageEffectParams.h"
#include "LightningBindActor.generated.h"

class UAbilitySystemComponent;

/// @todo FirenadoActor와 엮어서 범위 틱 데미지를 주는 Actor class로 리팩토링?

/**
 * Audio-Visual Effect를 적용하고, 실제로 데미지와 Stun을 가한다. 
 * UAuraAbility_LightningBind 어빌리티에 의해 생성된다.
 * Replication, Collision, Tick 등을 제외한 최소한의 기능만 수행한다.
 */
UCLASS()
class AURA_API ALightningBindActor : public AActor
{
	GENERATED_BODY()

public:
	ALightningBindActor();

	// 필요한 모든 데이터를 UAuraAbility_LightningBind에서 주입받는다.
	void Initialize(float InFinalEffectiveRadius, float InScaleRate, float InDamageInterval, int32 InTotalDamageCount, const FVector& InTargetLocation, const FDamageEffectParams& InDamageEffectParams, UAbilitySystemComponent* InSourceASC);

protected:
	virtual void BeginPlay() override;

private:
	/**
	 * UAuraAbility_LightningBind의 데이터를 Initialize() 함수로 주입받아 사용
	 * @see UAuraAbility_LightningBind
	 * @see Initialize()
	 */
	
	float FinalEffectiveRadius = 0.f;
	float ScaleRate = 0.f;
	float DamageInterval = 0.f;
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
	
};
