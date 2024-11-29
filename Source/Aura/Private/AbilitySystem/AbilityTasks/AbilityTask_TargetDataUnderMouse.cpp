// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"

UAbilityTask_TargetDataUnderMouse* UAbilityTask_TargetDataUnderMouse::CreateTask(UGameplayAbility* OwningAbility)
{
	UAbilityTask_TargetDataUnderMouse* Task = NewAbilityTask<UAbilityTask_TargetDataUnderMouse>(OwningAbility);
	return Task;
}

void UAbilityTask_TargetDataUnderMouse::Activate()
{
	// Client Player, Need to send it's data for server-side ability activation.
	// Host(if listen serer), already in server-side, so can use client's same logic 
	if (IsLocallyControlled())
	{
		SendTargetDataToServer();
	}
	// Server's non locally controlled Character (Not Player), Need to receive Client's Data
	else
	{
		const FGameplayAbilitySpecHandle& SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey& ActivationPredictionKey = GetActivationPredictionKey();
		
		// 서버에서 TargetData가 설정될 때의 Callback 함수를 바인딩한다.
		AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &ThisClass::OnAbilityTargetDataSet);

		// 이 시점에서 이미 클라이언트로부터 데이터를 받았으면 TargetDataSet Delegate 실행 -> 등록한 Callback 함수 OnAbilityTargetDataSet 호출
		if (!AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey))
		{
			// 아직 클라이언트의 데이터가 전송되지 않았다면(false) -> 받을 때까지 대기 -> 받으면 OnAbilityTargetDataSet 호출
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UAbilityTask_TargetDataUnderMouse::SendTargetDataToServer()
{
	// 해당 Scope 내의 작업을 Prediction
	FScopedPredictionWindow ScopedPredictionWindow(AbilitySystemComponent.Get());
	
	if (const APlayerController* PC = Ability->GetActorInfo().PlayerController.Get())
	{
		FHitResult HitResult;
		PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

		// FGameplayAbilityTargetData_SingleTargetHit 포인터는 FGameplayAbilityTargetDataHandle 내부에서 TSharedPtr로 괸리됨
		FGameplayAbilityTargetDataHandle TargetDataHandle(new FGameplayAbilityTargetData_SingleTargetHit(HitResult));

		// Valid한 TargetData를 Server로 전송
		AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), TargetDataHandle, FGameplayTag(), AbilitySystemComponent->ScopedPredictionKey);

		// AbilityTask를 실행한 Ability가 유효하고 Active한지 체크하고 Broadcast
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			TargetDataUnderMouseSet.Execute(TargetDataHandle);
		}
		EndTask();
	}
}

void UAbilityTask_TargetDataUnderMouse::OnAbilityTargetDataSet(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag ActivationTag)
{
	// Client에 전송받은 TargetData를 Clear
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	
	// AbilityTask를 실행한 Ability가 유효하고 Active한지 체크하고 Broadcast
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		TargetDataUnderMouseSet.Execute(TargetDataHandle);
	}
	EndTask();
}
