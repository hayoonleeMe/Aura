// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/AuraCharacter.h"
#include "Kismet/GameplayStatics.h"

UBTService_FindNearestPlayer::UBTService_FindNearestPlayer()
{
	Interval = 0.5f;
	RandomDeviation = 0.f;
}

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (const APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn())
	{
		// AuraCharacter 찾음
		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(OwnerPawn, AAuraCharacter::StaticClass(), AllActors);
		// 죽지 않은 Player를 찾는다.
		const TArray<AActor*> ActorsToCheck = AllActors.FilterByPredicate([](const AActor* Actor) { return !ICombatInterface::Execute_IsDead(Actor); });
		
		float DistanceToTarget = 0.f;
		AActor* NearestPlayer = UGameplayStatics::FindNearestActor(OwnerPawn->GetActorLocation(), ActorsToCheck, DistanceToTarget);

		// Blackboard 값 업데이트
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetActorSelector.SelectedKeyName, NearestPlayer);		
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(DistanceToTargetSelector.SelectedKeyName, DistanceToTarget);
	}
}
