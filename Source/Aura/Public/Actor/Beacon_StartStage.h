// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractionInterface.h"
#include "Beacon_StartStage.generated.h"

class UWidgetComponent;
class UBoxComponent;

/**
 * 다음 스테이지를 시작하는 상호작용이 가능한 Beacon Actor
 */
UCLASS()
class AURA_API ABeacon_StartStage : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	ABeacon_StartStage();

	/* Begin InteractionInterface */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	virtual void Interact() override;
	/* End InteractionInterface */

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UFUNCTION(Server, Reliable)
	void ServerInteract();

	// ============================================================================
	// Tooltip
	// ============================================================================

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> TooltipWidgetComponent;
};
