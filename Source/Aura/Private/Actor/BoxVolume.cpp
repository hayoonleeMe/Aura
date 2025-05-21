// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/BoxVolume.h"

#include "Components/BoxComponent.h"

ABoxVolume::ABoxVolume()
{
	SetReplicates(false);
	SetActorEnableCollision(false);
	SetCanBeDamaged(false);
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	SetRootComponent(BoxComponent);
	BoxComponent->InitBoxExtent(FVector(50.f, 50.f, 0.f));
	BoxComponent->SetMobility(EComponentMobility::Static);
}

FBox ABoxVolume::GetVolumeBounds() const
{
	const FVector Location = BoxComponent->GetComponentLocation();
	const FVector BoxExtent = BoxComponent->GetScaledBoxExtent();
	return FBox(Location - BoxExtent, Location + BoxExtent);
}
