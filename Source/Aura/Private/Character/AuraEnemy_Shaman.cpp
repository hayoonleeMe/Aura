// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraEnemy_Shaman.h"

#include "AI/AuraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AAuraEnemy_Shaman::AAuraEnemy_Shaman()
{
	SummonEffectiveRange = 1000.f;
}

void AAuraEnemy_Shaman::PossessedBy(AController* NewController)
{
	ACharacter::PossessedBy(NewController);

	// AI server only
	if (HasAuthority())
	{
		// 초기에 SummonEffectiveRange를 사용한다.
		AuraAIController = Cast<AAuraAIController>(NewController);
		AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		AuraAIController->GetBlackboardComponent()->SetValueAsFloat(TEXT("AttackEffectiveRange"), SummonEffectiveRange);
		AuraAIController->GetBlackboardComponent()->SetValueAsFloat(TEXT("AttackAbortRange"), AttackAbortRange);
		AuraAIController->RunBehaviorTree(BehaviorTree);
	}
}

void AAuraEnemy_Shaman::UpdateAttackEffectiveRange()
{
	// AI server only
	if (HasAuthority() && AuraAIController)
	{
		// Shaman Summon Ability가 종료되면 AttackEffectiveRange 값을 사용하도록 업데이트한다.
		AuraAIController->GetBlackboardComponent()->SetValueAsFloat(TEXT("AttackEffectiveRange"), AttackEffectiveRange);
	}
}
