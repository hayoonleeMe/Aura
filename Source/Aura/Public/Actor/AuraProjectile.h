// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/DamageEffectParams.h"
#include "AuraProjectile.generated.h"

class UProjectileMovementComponent;
class UNiagaraSystem;
class USphereComponent;

/**
 * Sphere Component Overlap Event가 발생하면 데미지를 입히고 Destroy되는 Projectile 
 */
UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()

public:
	AAuraProjectile();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	// for Apply Damage Effect
	FDamageEffectParams DamageEffectParams;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Projectile의 Overlap이 발생할 수 있는지 판별
	bool IsValidOverlap(const AActor* TargetActor) const;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;

private:
	// 충돌 감지 Component
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(EditAnywhere, Category="Aura")
	TObjectPtr<USoundBase> LoopingSound;

	// Actor의 수명
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	float LifeSpan;

	// Projectile의 Impact Effect를 표시하고 Impact Sound를 재생하는 GameplayCue의 GameplayTag
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	FGameplayTag ImpactCueTag;
};
