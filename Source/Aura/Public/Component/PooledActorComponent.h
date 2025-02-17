// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PooledActorComponent.generated.h"

// SetInUse가 호출될 때를 알리는 델레게이트
// 컴포넌트의 Owner에서 해당 델레게이트를 구독해 SetInUse 함수를 override 하는 것처럼 추가 작업을 수행할 수 있다.
DECLARE_DELEGATE_OneParam(FSetInUseSignature, bool /* bInUse */);

/**
 * Object Pool System을 위한 Actor Component
 * Object Pool로 관리할 Actor에 등록해 사용한다.
 * Actor의 사용 상태를 관리하고 Replicate한다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UPooledActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPooledActorComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetInUse(bool bNewInUse);
	bool IsInUse() const { return bInUse; }

	FSetInUseSignature SetInUseDelegate;

protected:
	virtual void BeginPlay() override;

private:
	// 사용 상태
	UPROPERTY(ReplicatedUsing=OnRep_InUse)
	uint8 bInUse : 1;
	
	UFUNCTION()
	void OnRep_InUse();
};
