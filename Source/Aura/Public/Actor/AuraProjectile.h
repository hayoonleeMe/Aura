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
	virtual void PostInitializeComponents() override;

	// for Apply Damage Effect
	FDamageEffectParams DamageEffectParams;

protected:
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Projectile의 Overlap이 발생할 수 있는지 판별
	virtual bool IsValidOverlap(const AActor* TargetActor) const;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	// 충돌 감지 Component
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;

	// Projectile이 날아가는 동안 재생할 Sound Effect
	UPROPERTY(VisibleAnywhere)
    TObjectPtr<UAudioComponent> LoopingSoundComponent;

	// Projectile의 Impact Effect를 표시하고 Impact Sound를 재생하는 GameplayCue의 GameplayTag
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	FGameplayTag ImpactCueTag;
};
