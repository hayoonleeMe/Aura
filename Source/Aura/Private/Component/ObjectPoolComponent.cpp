// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/ObjectPoolComponent.h"

#include "Component/PooledActorComponent.h"
#include "Net/UnrealNetwork.h"

UObjectPoolComponent::UObjectPoolComponent()
{
	SetIsReplicatedByDefault(true);
}

void UObjectPoolComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UObjectPoolComponent, Pool);
}

void UObjectPoolComponent::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서 생성 후 클라이언트로 Replicate
	// 항상 모든 기기로 Replicate 돼야 하므로 PooledActor는 bAlwaysRelevant = true여야 한다.
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		checkf(IsPooledActorClassValid(), TEXT("PooledActorClass instance must have UPooledActorComponent"));

		// PoolSize 만큼 생성해 저장해둔다.
		for (int32 Count = 0; Count < PoolSize; ++Count)
		{
			if (const AActor* PooledActor = GetWorld()->SpawnActor<AActor>(PooledActorClass, SpawnParams))
			{
				Pool.Add(PooledActor->FindComponentByClass<UPooledActorComponent>());
			}
		}
	}
}

bool UObjectPoolComponent::IsPooledActorClassValid() const
{
	if (PooledActorClass)
	{
		if (const AActor* CDO = PooledActorClass->GetDefaultObject<AActor>())
		{
			return CDO->FindComponentByClass<UPooledActorComponent>() != nullptr;
		}
	}
	return false;
}

UPooledActorComponent* UObjectPoolComponent::FindAvailable() const
{
	for (UPooledActorComponent* PooledActorComponent : Pool)
	{
		if (PooledActorComponent && !PooledActorComponent->IsInUse())
		{
			return PooledActorComponent;
		}
	}
	return nullptr;
}

AActor* UObjectPoolComponent::SpawnFromPool(const FTransform& SpawnTransform, bool bSetInUse) const
{
	UPooledActorComponent* PooledActorComponent = FindAvailable();
	
	if (!PooledActorComponent || !IsValid(PooledActorComponent->GetOwner()))
	{
		UE_LOG(LogTemp, Error, TEXT("[ %hs ] Failed to spawn actor from pool in %s"), __FUNCTION__, *GetName());
		return nullptr;	
	}

	AActor* Actor = Cast<AActor>(PooledActorComponent->GetOwner());
	if (Actor)
	{
		Actor->SetActorTransform(SpawnTransform);
	}

	if (bSetInUse)
	{
		PooledActorComponent->SetInUse(true);
	}
	
	return Actor;
}
