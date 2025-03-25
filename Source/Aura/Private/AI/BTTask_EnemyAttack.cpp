// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_EnemyAttack.h"

#include "AIController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/CombatInterface.h"

UBTTask_EnemyAttack::UBTTask_EnemyAttack()
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_EnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	check(AIController);

	UAuraAbilitySystemComponent* OwnerAuraASC = CastChecked<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AIController->GetPawn()));
	
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(AIController->GetPawn()))
	{
		// Enemy가 공격할 대상인 CombatTarget 설정
		AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));
		if (!IsValid(TargetActor))
		{
			return EBTNodeResult::Failed;
		}
		CombatInterface->SetCombatTarget(TargetActor);
	}

	if (const FGameplayAbilitySpec* AbilitySpec = OwnerAuraASC->GetSpellSpecForSpellTag(AuraGameplayTags::Abilities_EnemyAttack))
	{
		if (UGameplayAbility* Ability = AbilitySpec->GetPrimaryInstance())
		{
			// Enemy Attack Ability가 끝나면 Succeeded
			FDelegateHandle DelegateHandle = Ability->OnGameplayAbilityEnded.AddWeakLambda(this, [this, DelegateHandle, &OwnerComp](UGameplayAbility* EndedAbility)
			{
				if (EndedAbility)
				{
					EndedAbility->OnGameplayAbilityEnded.Remove(DelegateHandle);
				}
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			});	
		}

		// Enemy Attack Ability 실행
		if (!OwnerAuraASC->TryActivateAbility(AbilitySpec->Handle))
		{
			return EBTNodeResult::Failed;
		}

		// Ability가 끝나기 전
		return EBTNodeResult::InProgress;
	}
	
	return EBTNodeResult::Failed;
}
