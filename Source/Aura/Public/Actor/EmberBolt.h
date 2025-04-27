// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FireBolt.h"
#include "EmberBolt.generated.h"

/**
 * FireBall Projectile이 폭발한 뒤 주변으로 발사되는 EmberBolt Projectile
 */
UCLASS()
class AURA_API AEmberBolt : public AFireBolt
{
	GENERATED_BODY()

public:
	AEmberBolt();
};
