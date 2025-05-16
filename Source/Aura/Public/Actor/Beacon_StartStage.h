// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractionInterface.h"
#include "Beacon_StartStage.generated.h"

class UNavModifierComponent;
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
	virtual float GetOverrideArriveAcceptanceRadius() const override;
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

	// ============================================================================
	// Navigation
	// ============================================================================

	/**
	 * Beacon은 UNavModifierComponent을 통해 NavMesh에 Affect한다.
	 * UNavArea_Default와 동일한 UNavArea_Beacon을 사용해 항상 이동 가능하다.
	 * 해당 Beacon을 피해가야 할 때는 UAvoidBeaconQueryFilter를 사용해 쿼리할 수 있다. (UNavArea_Beacon에 대해 Exclude하는 Query Filter)
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNavModifierComponent> NavModifierComponent;
};
