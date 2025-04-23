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
 * 
 * 하나의 플레이어와 한번만 상호작용 할 수 있다.
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
	virtual float GetOverrideArriveAcceptanceRadius() const override { return 80.f; }
	virtual bool CanTryInteract() const override { return true; }
	/* End InteractionInterface */

	UFUNCTION(BlueprintImplementableEvent)
	void StartGlowTimeline(UMaterialInstanceDynamic* MaterialInstanceDynamic);

	UFUNCTION(BlueprintCallable)
	void OnGlowTimelineFinished();

	// 해당 액터를 숨기기 위해 MeshComponent의 HiddenInGame 변경
	// Replicate되지 않음
	void SetMeshComponentHiddenInGame(bool bNewHidden) const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	// for Glow Effect
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterialInstance;

	UFUNCTION(Server, Reliable)
	void ServerInteract();

	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<USoundBase> InteractSound;

	// 현재 기기의 플레이어와 상호작용 했는지
	bool bHasInteractedWithPlayer = false;

	// ============================================================================
	// Tooltip
	// ============================================================================

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> TooltipWidgetComponent;

	// ToolTip Widget의 Text가 제대로 설정됐는지
	bool bHasSetToolTipWidgetText = false;
};
