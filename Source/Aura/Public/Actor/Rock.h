// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PooledProjectile.h"
#include "Rock.generated.h"

/**
 * Rock Static Mesh Component와 Rock Impact Effect를 포함해 Rock을 나타내는 Projectile Class
 */
UCLASS()
class AURA_API ARock : public APooledProjectile
{
	GENERATED_BODY()

public:
	ARock();
	virtual void Tick(float DeltaTime) override;

private:
	// VFX
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	virtual void OnSetInUse(bool bInUse) override;
};
