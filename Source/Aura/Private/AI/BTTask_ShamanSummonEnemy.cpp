// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ShamanSummonEnemy.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/CombatInterface.h"

UBTTask_ShamanSummonEnemy::UBTTask_ShamanSummonEnemy()
{
	NodeName = TEXT("Activate Shaman Summon Enemy Ability");
}

EBTNodeResult::Type UBTTask_ShamanSummonEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	check(AIController);
	
	UAuraAbilitySystemComponent* OwnerAuraASC = CastChecked<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AIController->GetPawn()));

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(AIController->GetPawn()))
	{
		// 실제로 공격을 수행하지 않지만 MotionWarping을 위해 CombatTarget 설정
		AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorSelector.SelectedKeyName));
		if (!IsValid(TargetActor))
		{
			return EBTNodeResult::Failed;
		}
		CombatInterface->SetCombatTarget(TargetActor);
	}
	
	if (const FGameplayAbilitySpec* AbilitySpec = OwnerAuraASC->GetSpellSpecForSpellTag(AuraGameplayTags::Abilities_ShamanSummonEnemy))
	{
		if (UGameplayAbility* Ability = AbilitySpec->GetPrimaryInstance())
		{
			// Summon Enemy Ability가 끝나면 Succeeded하도록 등록
			FDelegateHandle DelegateHandle = Ability->OnGameplayAbilityEnded.AddWeakLambda(this, [this, DelegateHandle, &OwnerComp](UGameplayAbility* EndedAbility)
			{
				if (EndedAbility)
				{
					EndedAbility->OnGameplayAbilityEnded.Remove(DelegateHandle);
				}
				if (OwnerComp.GetBlackboardComponent())
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsBool(HasSummonedEnemySelector.SelectedKeyName, true);
				}
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			});	
		}

		// Summon Enemy Ability 실행
		OwnerAuraASC->TryActivateAbility(AbilitySpec->Handle);

		// Summon Enemy Ability가 끝나기 전
		return EBTNodeResult::InProgress;
	}
	
	return EBTNodeResult::Failed;
}
