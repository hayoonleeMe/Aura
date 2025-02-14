// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SpawnEnemyVolume.h"

#include "Components/BoxComponent.h"

ASpawnEnemyVolume::ASpawnEnemyVolume()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	SetRootComponent(BoxComponent);
	BoxComponent->InitBoxExtent(FVector(50.f, 50.f, 0.f));
	BoxComponent->SetMobility(EComponentMobility::Static);

	SetActorEnableCollision(false);
	bNetLoadOnClient = false;
}
