// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_EnemyAttack.h"

#include "AIController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interaction/CombatInterface.h"

UBTTask_EnemyAttack::UBTTask_EnemyAttack()
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_EnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	check(AIController);
	AActor* OwnerActor = AIController->GetPawn();
	if (IsValid(OwnerActor) && OwnerActor->Implements<UCombatInterface>())
	{
		UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
		check(OwnerASC);

		// Enemy가 공격할 대상인 CombatTarget 설정
		if (AActor* TargetActor = CastChecked<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey())))
		{
			ICombatInterface::Execute_SetCombatTarget(OwnerActor, TargetActor);
		}

		// Enemy의 공격을 나타내는 GameplayTag로 Attack Ability 실행
		const FGameplayTagContainer TagContainer(FAuraGameplayTags::Get().Abilities_EnemyAttack);
		if (!OwnerASC->TryActivateAbilitiesByTag(TagContainer))
		{
			return EBTNodeResult::Failed;
		}

		// Enemy Attack Ability가 끝나면 Succeeded
		FDelegateHandle AbilityEndedDelegateHandle = OwnerASC->OnAbilityEnded.AddWeakLambda(this, [this, TagContainer, OwnerASC, AbilityEndedDelegateHandle, &OwnerComp](const FAbilityEndedData& EndedData)
		{
			if (EndedData.AbilityThatEnded->AbilityTags.HasAllExact(TagContainer))
			{
				OwnerASC->OnAbilityEnded.Remove(AbilityEndedDelegateHandle);

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
		});
		
		// Enemy Attack Ability가 끝나기 전
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}