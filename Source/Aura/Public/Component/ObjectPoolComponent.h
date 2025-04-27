// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectPoolComponent.generated.h"

class UPooledActorComponent;

/**
 * APooledActor 타입의 Actor를 미리 생성하고, 필요할 때 사용할 수 있는 Actor를 전달해주는 Actor Component
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UObjectPoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UObjectPoolComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetPoolSize(int32 InPoolSize) { PoolSize = InPoolSize; }

	// PooledActors에서 사용할 수 있는 PooledActorComponent를 반환한다. 
	UPooledActorComponent* FindAvailable() const;

	// Object Pool에서 사용할 수 있는 Actor를 가져와 SpawnTransform을 설정하고 반환한다.
	// bSetInUse가 true이면, Pool에서 꺼내면서 바로 사용 중으로 설정한다.
	AActor* SpawnFromPool(const FTransform& SpawnTransform, bool bSetInUse) const;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	int32 PoolSize;

	// 생성해 Pool에 저장할 Actor 클래스
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TSubclassOf<AActor> PooledActorClass;

	// 생성한 PooledActorComponent를 저장하는 Pool
	UPROPERTY(Replicated)
	TArray<UPooledActorComponent*> Pool;

	bool IsPooledActorClassValid() const;
};
