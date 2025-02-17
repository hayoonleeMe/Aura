// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Rock.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

ARock::ARock()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComponent->SetSphereRadius(8.f);

	ProjectileMovementComponent->InitialSpeed = 1000.f;
	ProjectileMovementComponent->MaxSpeed = 1000.f;
	ProjectileMovementComponent->ProjectileGravityScale = 1.5f;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetRelativeScale3D(FVector(1.25f));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Random Rotate Effect
	MeshComponent->AddLocalRotation(UKismetMathLibrary::RandomRotator(true) * DeltaTime);
}

