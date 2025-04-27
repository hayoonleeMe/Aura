// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/EmberBolt.h"

#include "GameFramework/ProjectileMovementComponent.h"

AEmberBolt::AEmberBolt()
{
	ProjectileMovementComponent->InitialSpeed = 700.f;
	ProjectileMovementComponent->MaxSpeed = 700.f;

	PooledActorType = EPooledActorType::EmberBolt;
}
