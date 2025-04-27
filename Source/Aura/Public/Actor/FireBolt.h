// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PooledProjectile.h"
#include "FireBolt.generated.h"

class UNiagaraComponent;

/**
 * Fire Niagara System과 Fire Impact Effect를 포함해 FireBolt를 나타내는 Projectile Class
 */
UCLASS()
class AURA_API AFireBolt : public APooledProjectile
{
	GENERATED_BODY()

public:
	AFireBolt();

protected:
	// VFX
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> FireEffectComponent;

	virtual void OnSetInUse(bool bInUse) override;
};
