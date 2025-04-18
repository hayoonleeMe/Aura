// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FireBolt.h"
#include "Component/PooledActorComponent.h"
#include "FireBolt_Pooled.generated.h"

class UPooledActorComponent;
class UNiagaraComponent;

/**
 * Object Pool로 관리되는 FireBolt Projectile Class
 */
UCLASS()
class AURA_API AFireBolt_Pooled : public AFireBolt
{
	GENERATED_BODY()

public:
	AFireBolt_Pooled();
	virtual void PostInitializeComponents() override;

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual bool IsValidOverlap(const AActor* TargetActor) const override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPooledActorComponent> PooledActorComponent;

	void SetInUse(bool bInUse) const;
};
