// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AuraGameStateBase.generated.h"

class UAttributeConfig;
class ALevelSequenceActor;
class UAuraInputConfig;
class UInputMappingContext;

/**
 * 
 */
UCLASS()
class AURA_API AAuraGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

protected:
	virtual void PostInitializeComponents() override;
	
public:
	AAuraGameStateBase();
	
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UAuraInputConfig> AuraInputConfig;


	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<UAttributeConfig> AttributeConfig;

	// ============================================================================
	// Respawn
	// ============================================================================

	// 플레이어의 최대 목숨 개수
	UPROPERTY(EditDefaultsOnly, Category="Aura|Respawn")
	int32 TotalLifeCount;

	// 리스폰에 걸리는 시간
	UPROPERTY(EditDefaultsOnly, Category="Aura|Respawn")
	float RespawnTime;

	// ============================================================================
	// Level Sequence
	// ============================================================================

	// PauseMenu를 열 때 재생할 Level Sequence Actor로, Level에 배치된 액터를 캐싱한다.
	// 해당 액터에 설정된 Level Sequence의 Cine Camera는 해당 Level Sequence Actor에 부착되고 Enable Look at Tracking 옵션을 사용한다.
	// 이를 통해 해당 Level Sequence Actor의 부모 액터인 플레이어 캐릭터를 중심으로 바라보며 Level Sequence가 재생된다.
	UPROPERTY()
	TObjectPtr<ALevelSequenceActor> PauseMenuLevelSequenceActor;

	// Level에 배치된 PauseMenu Level Sequence Actor를 Pawn에 부착한다.
	void AttachPauseMenuLevelSequenceActorToPawn(APawn* Pawn) const;
};
