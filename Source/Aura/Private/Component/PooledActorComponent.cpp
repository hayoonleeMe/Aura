// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PooledActorComponent.h"

#include "Net/UnrealNetwork.h"


UPooledActorComponent::UPooledActorComponent()
{
	SetIsReplicatedByDefault(true);
	
	bInUse = false;
}

void UPooledActorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPooledActorComponent, bInUse);
}

void UPooledActorComponent::SetInUse(bool bNewInUse)
{
	AActor* OwningActor = GetOwner<AActor>();
	check(OwningActor);

	bInUse = bNewInUse;
	OwningActor->SetActorEnableCollision(bInUse);
	OwningActor->SetActorHiddenInGame(!bInUse);
	OwningActor->SetActorTickEnabled(bInUse);

	if (SetInUseDelegate.IsBound())
	{
		SetInUseDelegate.Execute(bInUse);
	}
}

void UPooledActorComponent::OnRep_InUse()
{
	SetInUse(bInUse);
}

void UPooledActorComponent::BeginPlay()
{
	Super::BeginPlay();

	SetInUse(false);
}
