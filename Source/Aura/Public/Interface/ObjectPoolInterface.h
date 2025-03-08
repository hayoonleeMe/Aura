// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/PooledActorType.h"
#include "UObject/Interface.h"
#include "ObjectPoolInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UObjectPoolInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Actor에 등록된 UObjectPoolComponent의 함수를 호출하는 인터페이스
 */
class AURA_API IObjectPoolInterface
{
	GENERATED_BODY()

public:
	// PooledActorType에 해당하는 Object Pool에서 사용할 수 있는 Actor를 가져와 SpawnTransform을 설정하고 반환한다.
	virtual AActor* SpawnFromPool(EPooledActorType PooledActorType, const FTransform& SpawnTransform) { return nullptr; }

	template<class T>
	T* SpawnFromPool(EPooledActorType PooledActorType, const FTransform& SpawnTransform)
	{
		return Cast<T>(SpawnFromPool(PooledActorType, SpawnTransform));
	}
};
