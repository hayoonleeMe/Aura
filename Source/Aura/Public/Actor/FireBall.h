// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PooledProjectile.h"
#include "FireBall.generated.h"

class UNiagaraComponent;

/**
 * 폭발 후 사방으로 EmberBolt를 발사하는 FireBall Projectile
 */
UCLASS()
class AURA_API AFireBall : public APooledProjectile
{
	GENERATED_BODY()

public:
	AFireBall();

	// FireBall 폭발 후 EmberBolt를 Spawn하기 위해 옵션 설정
	void SetEmberBoltOptions(int32 InNumEmberBolts, float InEmberBoltDamage, const TSubclassOf<AAuraProjectile>& InEmberBoltClass);

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> FireEffectComponent;

	int32 NumEmberBolts = 0;
	float EmberBoltDamage = 0.f;
	TSubclassOf<AAuraProjectile> EmberBoltClass;

	void SpawnEmberBolts() const;

	virtual void SetInUse(bool bInUse) override;
};
