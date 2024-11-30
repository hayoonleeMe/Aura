// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_TargetDataUnderMouse.generated.h"

DECLARE_DELEGATE_OneParam(FTargetDataUnderMouseSetSignature, const FGameplayAbilityTargetDataHandle& /* TargetDataHandle */);

/**
 * 마우스 커서의 HitResult를 얻는 AbilityTask
 * Locally Controlled Owner인 경우, 해당 기기의 월드에서 수행한 Trace 결과를 반환하도록 구현
 * 서버(Non Locally Controlled Owner)에서는 클라이언트에서의 데이터를 전송받아 반환하도록 구현
 */
UCLASS()
class AURA_API UAbilityTask_TargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()

public:
	static UAbilityTask_TargetDataUnderMouse* CreateTask(UGameplayAbility* OwningAbility);

	// 올바른 TargetData(HitResult)를 반환할 때 실행되는 델레게이트
	FTargetDataUnderMouseSetSignature TargetDataUnderMouseSetDelegate;
	
protected:
	virtual void Activate() override;

private:
	// Client의 Valid한 TargetData를 Server로 전송
	void SendTargetDataToServer();

	// Server에서 Client로부터 Valid한 TargetData를 전송받아 TargetDataSet Delegate가 실행될 때 호출되는 Callback 함수
	void OnAbilityTargetDataSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag ActivationTag);
};
