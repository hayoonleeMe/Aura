// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraEnemy.h"
#include "AuraEnemy_Shaman.generated.h"

UCLASS()
class AURA_API AAuraEnemy_Shaman : public AAuraEnemy
{
	GENERATED_BODY()

public:
	AAuraEnemy_Shaman();
	virtual void PossessedBy(AController* NewController) override;

	// Shaman Summon Enemy Ability가 종료되어 AttackEffectiveRange 값을 업데이트한다.
	void UpdateAttackEffectiveRange();
	
private:
	// Shaman Summon Enemy Ability 실행 여부를 판단할 때 사용하는 거리
	UPROPERTY(EditAnywhere, Category="Aura|Combat")
	float SummonEffectiveRange;
};
