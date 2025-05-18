// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraProjectile.h"
#include "Types/PooledActorType.h"
#include "PooledProjectile.generated.h"

class UPooledActorComponent;

/**
 * Object Pool에 의해 관리되는 Projectile Actor Class
 */
UCLASS()
class AURA_API APooledProjectile : public AAuraProjectile
{
	GENERATED_BODY()

public:
	APooledProjectile();
	
	FORCEINLINE EPooledActorType GetPooledActorType() const { return PooledActorType; }

	void SetInUse(bool bInUse) const;

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual bool IsValidOverlap(const AActor* TargetActor) const override;
	virtual void HandleProjectileOverlap(AActor* OtherActor);
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPooledActorComponent> PooledActorComponent;

	// UPooledActorComponent::SetInUse()가 호출될 때 같이 호출되는 콜백 함수
	// Object Pool에서 꺼내져 사용되거나 다시 Pool로 들어갈 때의 작업을 처리한다.
	virtual void OnSetInUse(bool bInUse);

	UPROPERTY(EditDefaultsOnly)
	EPooledActorType PooledActorType;
};
